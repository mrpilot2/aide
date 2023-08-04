
#include "mainwindow.hpp"

#include <utility>

#include <QCheckBox>
#include <QLayout>
#include <QMessageBox>
#include <QObject>
#include <QPushButton>

#include "actionregistry.hpp"
#include "aideconstants.hpp"
#include "applicationtranslator.hpp"
#include "hierarchicalid.hpp"
#include "mainwindowcontroller.hpp"
#include "menucontainerinterface.hpp"
#include "settings/settingsdialog.hpp"
#include "ui_mainwindow.h"

using aide::constants::CONSTANTS;
using aide::core::UserSelection;
using aide::gui::MainWindow;
using aide::gui::MainWindowControllerPtr;
using aide::gui::TranslatorInterface;

void initIconResource()
{
    Q_INIT_RESOURCE(icons);
}

MainWindow::MainWindow(LoggerPtr loggerInterface, QWidget* parent)
    : MainWindowInterface(parent)
    , logger{std::move(loggerInterface)}
    , m_translator{new ApplicationTranslator(logger)}
    , m_ui(new Ui::MainWindow)
{
    initIconResource();

    m_ui->setupUi(this);
}

MainWindow::~MainWindow() = default;

void MainWindow::setMainWindowController(
    MainWindowControllerPtr controller,
    const ActionRegistryInterfacePtr& actionRegistry)
{
    m_controller = std::move(controller);
    registerActions(actionRegistry);
}

void MainWindow::restoreGeometryAndState(QByteArray geometry, QByteArray state)
{
    this->restoreGeometry(geometry);
    this->restoreState(state);
}

void MainWindow::registerActions(
    const ActionRegistryInterfacePtr& actionRegistry)
{
    auto menuFileContainer{
        actionRegistry->createMenu(CONSTANTS().MENU_FILE, m_ui->menubar)};
    auto* menuFile{menuFileContainer->menu()};
    menuFile->setTitle(QApplication::tr("&File", "MainWindow"));

    m_actionSettings = std::make_shared<QAction>(tr("Settings"), this);
    connect(m_actionSettings.get(), &QAction::triggered, m_controller.get(),
            &MainWindowController::onUserWantsToShowSettingsDialog);
    menuFile->addAction(m_actionSettings.get());

    actionRegistry->registerAction(
        m_actionSettings, CONSTANTS().FILE_SETTINGS,
        tr("Edit application settings").toStdString(),
        {QKeySequence(Qt::CTRL | Qt::ALT | Qt::Key_S)});
    menuFile->addSeparator();

    m_actionQuit = std::make_shared<QAction>(
        createIconFromTheme("application-exit"), tr("Quit"), this);
    connect(m_actionQuit.get(), &QAction::triggered, QApplication::instance(),
            &QApplication::quit);
    menuFile->addAction(m_actionQuit.get());

    actionRegistry->registerAction(
        m_actionQuit, CONSTANTS().FILE_QUIT,
        tr("Quits the application").toStdString(),
        {QKeySequence(QKeySequence::Quit), QKeySequence("Alt+F4")});

    m_ui->menubar->addMenu(menuFile);

    auto menuHelpContainer{
        actionRegistry->createMenu(CONSTANTS().MENU_HELP, m_ui->menubar)};
    auto* menuHelp{menuHelpContainer->menu()};
    menuHelp->setTitle(QApplication::tr("&Help", "MainWindow"));

    m_actionAboutAide = std::make_shared<QAction>(tr("About") + " aIDE", this);
    connect(m_actionAboutAide.get(), &QAction::triggered, m_controller.get(),
            &MainWindowController::onUserWantsToShowAboutAideDialog);
    menuHelp->addAction(m_actionAboutAide.get());
    actionRegistry->registerAction(m_actionAboutAide,
                                   CONSTANTS().HELP_ABOUT_AIDE);

    m_actionAboutQt = std::make_shared<QAction>(tr("About Qt"), this);
    connect(m_actionAboutQt.get(), &QAction::triggered,
            QApplication::instance(), &QApplication::aboutQt);
    menuHelp->addAction(m_actionAboutQt.get());

    actionRegistry->registerAction(m_actionAboutQt, CONSTANTS().HELP_ABOUT_QT);
    m_ui->menubar->addMenu(menuHelp);
}

std::shared_ptr<TranslatorInterface> MainWindow::translator() const
{
    return m_translator;
}

QIcon MainWindow::createIconFromTheme(const std::string& iconName)
{
    QIcon icon;
    QString iconThemeName = QString::fromStdString(iconName);
    if (QIcon::hasThemeIcon(iconThemeName)) {
        icon = QIcon::fromTheme(iconThemeName);
    } else {
        icon.addFile(QString::fromUtf8(""), QSize(), QIcon::Normal, QIcon::Off);
    }
    return icon;
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    logger->trace("User requested to close application");
    m_controller->onUserWantsToQuitApplication(event, this->saveGeometry(),
                                               this->saveState());
}
std::tuple<aide::core::UserSelection, bool>
MainWindow::letUserConfirmApplicationClose()
{
    logger->debug("Asking user for confirmation to close application");
    auto messageBox = std::make_unique<QMessageBox>(this);

    auto checkBox =
        std::make_unique<QCheckBox>(tr("Don't ask again"), messageBox.get());

    messageBox->setWindowTitle(tr("Confirm exit"));
    messageBox->setText(tr("Are you sure you want to exit?"));
    messageBox->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    messageBox->setDefaultButton(QMessageBox::Yes);
    messageBox->setIcon(QMessageBox::Question);

    auto* layout = dynamic_cast<QGridLayout*>(messageBox->layout());
    if (layout != nullptr) { layout->addWidget(checkBox.get(), 2, 0); }
    auto reply = messageBox->exec();

    logger->debug("User requested to{} ask for exit confirmation again",
                  checkBox->isChecked() ? " do not" : "");

    return std::make_tuple(
        reply == QMessageBox::Yes ? UserSelection::Exit : UserSelection::Cancel,
        checkBox->isChecked());
}
