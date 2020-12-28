
#include "mainwindow.hpp"

#include <utility>

#include <QCheckBox>
#include <QLayout>
#include <QMessageBox>
#include <QObject>
#include <QPushButton>

#include <aide/log_helper_macros.hpp>

#include "actionregistry.hpp"
#include "applicationtranslator.hpp"
#include "hierarchicalid.hpp"
#include "mainwindowcontroller.hpp"
#include "settingsdialog.hpp"
#include "ui_mainwindow.h"

using aide::HierarchicalId;
using aide::core::MainWindowInterface;
using aide::core::UserSelection;
using aide::gui::MainWindow;
using aide::gui::MainWindowControllerPtr;
using aide::gui::TranslatorInterface;

struct ActionIds
{
    const HierarchicalId MAIN_MENU_FILE_SETTINGS{
        HierarchicalId("Main Menu")("File")("Settings")};
    const HierarchicalId MAIN_MENU_FILE_QUIT{
        HierarchicalId("Main Menu")("File")("Quit")};
    const HierarchicalId MAIN_MENU_HELP_ABOUT_QT{
        HierarchicalId("Main Menu")("Help")("About Qt")};
};

const static ActionIds& ACTION_IDS()
{
    try {
        static ActionIds ids;
        return ids;
    }
    catch (...) {
        AIDE_LOG_CRITICAL(
            "MainWindow: could not create hierarchical menu Ids with static "
            "storage duration. This should only happen if the application is "
            "already out of memory at startup")
        std::terminate();
    }
}

MainWindow::MainWindow(const ActionRegistryInterfacePtr& actionRegistry,
                       LoggerPtr loggerInterface, QWidget* parent)
    : MainWindowInterface(parent)
    , logger{std::move(loggerInterface)}
    , m_translator{new ApplicationTranslator(logger)}
    , m_ui(new Ui::MainWindow)
{
    m_ui->setupUi(this);

    registerActions(actionRegistry);
}

MainWindow::~MainWindow() = default;

void MainWindow::setMainWindowController(MainWindowControllerPtr controller)
{
    m_controller = std::move(controller);
}

void MainWindow::restoreGeometryAndState(QByteArray geometry, QByteArray state)
{
    this->restoreGeometry(geometry);
    this->restoreState(state);
}

void MainWindow::registerActions(
    const ActionRegistryInterfacePtr& actionRegistry)
{
    m_actionSettings = std::make_shared<QAction>(tr("Settings"), this);
    connect(m_actionSettings.get(), &QAction::triggered, this,
            &MainWindow::showSettingsDialog);
    m_ui->menuFile->addAction(m_actionSettings.get());

    actionRegistry->registerAction(
        m_actionSettings, ACTION_IDS().MAIN_MENU_FILE_SETTINGS,
        tr("Edit application settings").toStdString(),
        {QKeySequence(Qt::CTRL + Qt::ALT + Qt::Key_S)});
    m_ui->menuFile->addSeparator();

    m_actionQuit = std::make_shared<QAction>(
        createIconFromTheme("application-exit"), tr("Quit"), this);
    connect(m_actionQuit.get(), &QAction::triggered, QApplication::instance(),
            &QApplication::quit);
    m_ui->menuFile->addAction(m_actionQuit.get());

    actionRegistry->registerAction(
        m_actionQuit, ACTION_IDS().MAIN_MENU_FILE_QUIT,
        tr("Quits the application").toStdString(),
        {QKeySequence(QKeySequence::Quit), QKeySequence("Alt+F4")});

    m_actionAboutQt = std::make_shared<QAction>(tr("About Qt"), this);
    connect(m_actionAboutQt.get(), &QAction::triggered,
            QApplication::instance(), &QApplication::aboutQt);
    m_ui->menuHelp->addAction(m_actionAboutQt.get());

    actionRegistry->registerAction(m_actionAboutQt,
                                   ACTION_IDS().MAIN_MENU_HELP_ABOUT_QT);
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

void MainWindow::showSettingsDialog()
{
    auto settingsDialog = std::make_unique<SettingsDialog>(this);

    settingsDialog->exec();
}
