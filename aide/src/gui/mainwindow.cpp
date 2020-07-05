
#include "mainwindow.hpp"

#include <QObject>
#include <QPushButton>

#include "actionregistry.hpp"
#include "applicationtranslator.hpp"
#include "hierarchicalid.hpp"
#include "ui_mainwindow.h"

using aide::ActionRegistry;
using aide::HierarchicalId;
using aide::gui::MainWindow;
using aide::gui::TranslatorInterface;

static const auto MAIN_MENU_FILE_QUIT{
    HierarchicalId("Main Menu")("File")("Quit")};
static const auto MAIN_MENU_HELP_ABOUT_QT{
    HierarchicalId("Main Menu")("Help")("About Qt")};

MainWindow::MainWindow(std::shared_ptr<ActionRegistry> actionRegistry,
                       QWidget* parent)
    : QMainWindow(parent)
    , m_translator{new ApplicationTranslator}
    , m_ui(new Ui::MainWindow)
{
    m_ui->setupUi(this);

    registerActions(actionRegistry);
}

MainWindow::~MainWindow() = default;

void MainWindow::registerActions(
    const std::shared_ptr<ActionRegistry>& actionRegistry)
{
    m_actionQuit =
        std::make_shared<QAction>(createIconFromTheme("application-exit"),
                                  QApplication::tr("Quit"), this);
    connect(m_actionQuit.get(), &QAction::triggered, QApplication::instance(),
            &QApplication::quit);
    m_ui->menuFile->addAction(m_actionQuit.get());

    actionRegistry->registerAction(
        m_actionQuit, MAIN_MENU_FILE_QUIT,
        QApplication::tr("Quits the application").toStdString(),
        QKeySequence(QKeySequence::Quit));

    m_actionAboutQt =
        std::make_shared<QAction>(QApplication::tr("About Qt"), this);
    connect(m_actionAboutQt.get(), &QAction::triggered,
            QApplication::instance(), &QApplication::aboutQt);
    m_ui->menuHelp->addAction(m_actionAboutQt.get());

    actionRegistry->registerAction(m_actionAboutQt, MAIN_MENU_HELP_ABOUT_QT);
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
