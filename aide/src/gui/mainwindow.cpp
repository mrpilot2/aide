
#include "mainwindow.hpp"

#include <algorithm>
#include <utility>

#include <QCheckBox>
#include <QEvent>
#include <QFrame>
#include <QLayout>
#include <QMenu>
#include <QMessageBox>
#include <QObject>
#include <QPushButton>
#include <QString>
#include <QVBoxLayout>
#include <QWidget>

#include <aide/gui/widgets/notificationview.hpp>
#include <aide/hierarchicalid.hpp>

#include "actionregistry.hpp"
#include "aide/gui/widgets/banner.hpp"
#include "aideconstants.hpp"
#include "applicationtranslator.hpp"
#include "mainwindowcontroller.hpp"
#include "menucontainerinterface.hpp"
#include "osfilemanagerlauncher.hpp"
#include "settings/settingsdialog.hpp"
#include "ui_mainwindow.h"

using aide::HierarchicalId;
using aide::NotificationType;
using aide::constants::CONSTANTS;
using aide::core::UserSelection;
using aide::gui::MainWindow;
using aide::gui::MainWindowControllerPtr;
using aide::gui::TranslatorInterface;
using aide::widgets::Banner;
using aide::widgets::NotificationView;

extern int qInitResources_icons();

MainWindow::MainWindow(LoggerPtr loggerInterface, ApplicationConfig config,
                       QWidget* parent)
    : MainWindowInterface(parent)
    , logger{std::move(loggerInterface)}
    , m_config{std::move(config)}
    , m_translator{std::make_shared<ApplicationTranslator>(logger)}
    , m_ui(new Ui::MainWindow)
{
    qInitResources_icons();

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

void MainWindow::restoreGeometryAndState(const QByteArray geometry,
                                         const QByteArray state)
{
    this->restoreGeometry(geometry);
    this->restoreState(state);
}

void MainWindow::registerActions(
    const ActionRegistryInterfacePtr& actionRegistry)
{
    const auto* menuFileContainer{
        actionRegistry->createMenu(CONSTANTS().MENU_FILE, m_ui->menubar)};
    auto* menuFile{menuFileContainer->menu()};
    menuFile->setTitle(QApplication::tr("&File", "MainWindow"));

    m_actionSettings = new QAction(tr("Settings"), this);
    connect(m_actionSettings, &QAction::triggered, m_controller.get(),
            &MainWindowController::onUserWantsToShowSettingsDialog);
    menuFile->addAction(m_actionSettings);

    actionRegistry->registerAction(
        m_actionSettings, CONSTANTS().FILE_SETTINGS,
        tr("Edit application settings").toStdString(),
        {QKeySequence(Qt::CTRL | Qt::ALT | Qt::Key_S)});
    menuFile->addSeparator();

    m_actionQuit =
        new QAction(createIconFromTheme("application-exit"), tr("Quit"), this);
    connect(m_actionQuit, &QAction::triggered, QApplication::instance(),
            &QApplication::quit);
    menuFile->addAction(m_actionQuit);

    actionRegistry->registerAction(
        m_actionQuit, CONSTANTS().FILE_QUIT,
        tr("Quits the application").toStdString(),
        {QKeySequence(QKeySequence::Quit), QKeySequence("Alt+F4")});

    m_ui->menubar->addMenu(menuFile);

    registerViewMenu(actionRegistry);

    const auto* menuHelpContainer{
        actionRegistry->createMenu(CONSTANTS().MENU_HELP, m_ui->menubar)};
    auto* menuHelp{menuHelpContainer->menu()};
    menuHelp->setTitle(QApplication::tr("&Help", "MainWindow"));

    registerGatedHelpActions(menuHelp, actionRegistry);

    m_actionAboutAide = new QAction(tr("About") + " aIDE", this);
    connect(m_actionAboutAide, &QAction::triggered, m_controller.get(),
            &MainWindowController::onUserWantsToShowAboutAideDialog);
    menuHelp->addAction(m_actionAboutAide);
    actionRegistry->registerAction(m_actionAboutAide,
                                   CONSTANTS().HELP_ABOUT_AIDE);

    m_actionAboutQt = new QAction(tr("About Qt"), this);
    connect(m_actionAboutQt, &QAction::triggered, QApplication::instance(),
            &QApplication::aboutQt);
    menuHelp->addAction(m_actionAboutQt);

    actionRegistry->registerAction(m_actionAboutQt, CONSTANTS().HELP_ABOUT_QT);
    m_ui->menubar->addMenu(menuHelp);
}

void MainWindow::registerViewMenu(
    const ActionRegistryInterfacePtr& actionRegistry)
{
    // Gating: not created, not registered. When the feature is disabled the
    // action is neither built nor registered, and the View menu - which would
    // otherwise be empty - is not created either, so it leaves no trace in the
    // menu bar or the key-binding editor.
    if (!m_config.isEnabled(ApplicationConfig::Feature::ViewFullscreenAction)) {
        return;
    }

    const auto* menuViewContainer{
        actionRegistry->createMenu(CONSTANTS().MENU_VIEW, m_ui->menubar)};
    auto* menuView{menuViewContainer->menu()};
    menuView->setTitle(QApplication::tr("&View", "MainWindow"));

    m_actionFullScreen = new QAction(tr("Full Screen"), this);
    m_actionFullScreen->setCheckable(true);
    m_actionFullScreen->setChecked(
        windowState().testFlag(Qt::WindowFullScreen));
    connect(m_actionFullScreen, &QAction::triggered, this,
            &MainWindow::toggleFullScreen);
    menuView->addAction(m_actionFullScreen);

    actionRegistry->registerAction(m_actionFullScreen,
                                   CONSTANTS().VIEW_FULLSCREEN,
                                   tr("Toggle full screen mode").toStdString(),
                                   {QKeySequence(Qt::Key_F11)});

    m_ui->menubar->addMenu(menuView);
}

void MainWindow::registerGatedHelpActions(
    QMenu* menuHelp, const ActionRegistryInterfacePtr& actionRegistry)
{
    // Gating: not created, not registered, mirroring registerViewMenu above.
    // These features default to disabled since they are only appropriate
    // for developer-facing consumer applications. Both gated actions share
    // one trailing separator, added only if at least one of them was
    // actually registered, so the Help menu never shows a stray separator
    // before "About Aide" / "About Qt".
    bool anyGatedActionRegistered = false;

    if (m_config.isEnabled(
            ApplicationConfig::Feature::ShowLogInFileManagerAction)) {
        const core::OsFileManagerLauncher launcher;
        m_actionShowLogInFileManager = new QAction(
            tr("Show Log in %1")
                .arg(QString::fromStdString(launcher.displayName())),
            this);
        connect(m_actionShowLogInFileManager, &QAction::triggered,
                m_controller.get(),
                &MainWindowController::onUserWantsToShowLogInFileManager);
        menuHelp->addAction(m_actionShowLogInFileManager);
        actionRegistry->registerAction(
            m_actionShowLogInFileManager,
            CONSTANTS().HELP_SHOW_LOG_IN_FILE_MANAGER);
        anyGatedActionRegistered = true;
    }

    if (m_config.isEnabled(ApplicationConfig::Feature::ReportBugAction)) {
        m_actionReportBug = new QAction(tr("Report Bug in aIDE"), this);
        connect(m_actionReportBug, &QAction::triggered, m_controller.get(),
                &MainWindowController::onUserWantsToReportBug);
        menuHelp->addAction(m_actionReportBug);
        actionRegistry->registerAction(m_actionReportBug,
                                       CONSTANTS().HELP_REPORT_BUG);
        anyGatedActionRegistered = true;
    }

    if (anyGatedActionRegistered) { menuHelp->addSeparator(); }
}

void MainWindow::toggleFullScreen()
{
    // Toggle only the full-screen flag so the underlying maximized/normal
    // state is preserved: leaving full screen returns the window to whatever
    // it was before, in particular a maximized window stays maximized.
    // QWidget::showFullScreen() would instead clear the maximized flag, so it
    // is deliberately not used here.
    setWindowState(windowState() ^ Qt::WindowFullScreen);
}

void MainWindow::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::WindowStateChange &&
        m_actionFullScreen != nullptr) {
        // Drive the checkmark from the real window state so it stays truthful
        // whether the change came from F11, from geometry restore on launch,
        // or from the window manager.
        m_actionFullScreen->setChecked(
            windowState().testFlag(Qt::WindowFullScreen));
    }
    MainWindowInterface::changeEvent(event);
}

std::shared_ptr<TranslatorInterface> MainWindow::translator() const
{
    return m_translator;
}

void MainWindow::refreshIcons()
{
    if (m_actionQuit != nullptr) {
        m_actionQuit->setIcon(createIconFromTheme("application-exit"));
    }
}

QIcon MainWindow::createIconFromTheme(const std::string& iconName)
{
    QIcon icon;
    if (const QString iconThemeName = QString::fromStdString(iconName);
        QIcon::hasThemeIcon(iconThemeName)) {
        icon = QIcon::fromTheme(iconThemeName);
    } else {
        icon.addFile(QString::fromUtf8(""), QSize(), QIcon::Normal, QIcon::Off);
    }
    return icon;
}

Banner* MainWindow::addBanner(NotificationType type, const QString& message)
{
    ensureBannerHost();

    auto* banner = new Banner(type, message, m_bannerHost);
    connect(banner, &Banner::closed, this,
            [this, banner]() { removeBanner(banner); });

    m_banners.push_back(banner);
    rebuildBannerHostLayout();

    return banner;
}

void MainWindow::removeBanner(Banner* banner)
{
    const auto it = std::ranges::find(m_banners, banner);
    if (it == m_banners.end()) { return; }

    m_banners.erase(it);
    // Detach immediately rather than waiting for the deferred deletion to
    // run, so the banner is no longer reachable (e.g. via findChildren())
    // as soon as removeBanner() returns.
    banner->setParent(nullptr);
    banner->deleteLater();
    rebuildBannerHostLayout();
}

void MainWindow::createNotificationLogView(
    aide::NotificationManagerInterface& manager,
    aide::SettingsInterface& settings)
{
    m_notificationLogView = new NotificationView(manager, settings, this);

    connect(m_notificationLogView, &NotificationView::settingsRequested,
            m_controller.get(),
            &MainWindowController::onUserWantsToShowNotificationSettings);
    connect(m_notificationLogView, &NotificationView::groupSettingsRequested,
            m_controller.get(), [this](const HierarchicalId&) {
                m_controller->onUserWantsToShowNotificationSettings();
            });
}

aide::widgets::NotificationView* MainWindow::notificationLogView() const
{
    return m_notificationLogView;
}

void MainWindow::ensureBannerHost()
{
    // The banner host wraps whatever is currently the central widget the
    // first time a banner is requested, so it works whether the consumer
    // set a central widget before or never at all. QMainWindow's own
    // setCentralWidget() is not virtual and cannot be intercepted, so a
    // consumer replacing the central widget again after banners exist would
    // discard this wrapper along with any active banners - an inherent
    // QMainWindow single-owner constraint, not something addBanner() can
    // guard against.
    if (m_bannerWrapper != nullptr) { return; }

    auto* previousCentralWidget = centralWidget();

    m_bannerWrapper     = new QWidget(this);
    auto* wrapperLayout = new QVBoxLayout(m_bannerWrapper);
    wrapperLayout->setContentsMargins(0, 0, 0, 0);
    wrapperLayout->setSpacing(0);

    m_bannerHost       = new QWidget(m_bannerWrapper);
    m_bannerHostLayout = new QVBoxLayout(m_bannerHost);
    m_bannerHostLayout->setContentsMargins(0, 0, 0, 0);
    m_bannerHostLayout->setSpacing(0);

    wrapperLayout->addWidget(m_bannerHost);
    if (previousCentralWidget != nullptr) {
        wrapperLayout->addWidget(previousCentralWidget, 1);
    }

    setCentralWidget(m_bannerWrapper);
}

void MainWindow::rebuildBannerHostLayout()
{
    while (QLayoutItem* item = m_bannerHostLayout->takeAt(0)) {
        auto* widget = item->widget();
        delete item;
        const bool isBanner =
            std::ranges::find(m_banners, widget) != m_banners.cend();
        if (widget != nullptr && !isBanner) { widget->deleteLater(); }
    }

    for (std::size_t i = 0; i < m_banners.size(); ++i) {
        if (i > 0) {
            auto* divider = new QFrame(m_bannerHost);
            divider->setFrameShape(QFrame::HLine);
            divider->setFrameShadow(QFrame::Plain);
            m_bannerHostLayout->addWidget(divider);
        }
        m_bannerHostLayout->addWidget(m_banners[i]);
    }
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    logger->trace("User requested to close application");
    m_controller->onUserWantsToQuitApplication(event, this->saveGeometry(),
                                               this->saveState());
}
std::tuple<UserSelection, bool> MainWindow::letUserConfirmApplicationClose()
{
    logger->debug("Asking user for confirmation to close application");
    const auto messageBox = std::make_unique<QMessageBox>(this);

    const auto checkBox =
        std::make_unique<QCheckBox>(tr("Don't ask again"), messageBox.get());

    messageBox->setWindowTitle(tr("Confirm exit"));
    messageBox->setText(tr("Are you sure you want to exit?"));
    messageBox->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    messageBox->setDefaultButton(QMessageBox::Yes);
    messageBox->setIcon(QMessageBox::Question);

    if (auto* layout = dynamic_cast<QGridLayout*>(messageBox->layout());
        layout != nullptr) {
        layout->addWidget(checkBox.get(), 2, 0);
    }
    const auto reply = messageBox->exec();

    logger->debug("User requested to{} ask for exit confirmation again",
                  checkBox->isChecked() ? " do not" : "");

    return std::make_tuple(
        reply == QMessageBox::Yes ? UserSelection::Exit : UserSelection::Cancel,
        checkBox->isChecked());
}
