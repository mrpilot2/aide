#include <QAction>
#include <QByteArray>
#include <QDir>
#include <QLabel>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QPalette>
#include <QSplitter>
#include <QString>

#include <aide/aideconstants.hpp>
#include <aide/aidesettingsprovider.hpp>
#include <aide/application.hpp>
#include <aide/colorscheme.hpp>
#include <aide/gui/translatorinterface.hpp>
#include <aide/gui/widgets/notificationview.hpp>
#include <aide/hierarchicalid.hpp>
#include <aide/menucontainerinterface.hpp>
#include <aide/notificationdisplaytype.hpp>
#include <aide/notificationgroup.hpp>
#include <aide/settingsinterface.hpp>
#include <aide/theme.hpp>

#include "colorschemereactor.hpp"
#include "demosettingspage.hpp"
#include "notificationlauncherdialog.hpp"
#include "reportbugpreviewlauncher.hpp"

using aide::constants::CONSTANTS;

int main(int argc, char* argv[])
{
    aide::Application::setOrganizationName("aide_company");
    aide::Application::setApplicationName("Fancy_Aide_Demo");
    aide::Application::setApplicationDisplayName("Fancy Aide Demo");

    // The demo keeps every built-in feature enabled and passes the config
    // explicitly to show the API. A consumer that did not want the
    // View -> Full Screen action would disable it here, e.g.:
    //   config.setEnabled(
    //       aide::ApplicationConfig::Feature::ViewFullscreenAction, false);
    //
    // Two exceptions: ShowLogInFileManagerAction and ReportBugAction default
    // to disabled since they are only appropriate for developer-facing
    // consumer applications. The demo opts in explicitly so both features
    // are visibly exercised by anyone running it.
    //
    // ReportBugAction also gets a demo-supplied UrlLauncherInterface
    // override, so clicking the action shows a preview dialog of the ticket
    // that would have been filed instead of opening a real browser and
    // filing a real issue against aIDE's own repository.
    aide::ApplicationConfig config;
    config.setEnabled(
        aide::ApplicationConfig::Feature::ShowLogInFileManagerAction, true);
    config.setEnabled(aide::ApplicationConfig::Feature::ReportBugAction, true);
    config.setUrlLauncher(std::make_shared<demo::ReportBugPreviewLauncher>());
    const aide::Application app(argc, argv, config);

    app.translator()->addAdditionalTranslationFilePath(
        QDir(":/demo_translations"), QString("demo"));

    // Register a consumer-defined theme
    constexpr int blueWindowR{220};
    constexpr int blueWindowG{235};
    constexpr int blueWindowB{255};
    constexpr int blueButtonR{200};
    constexpr int blueButtonG{220};
    constexpr int blueButtonB{255};
    QPalette bluePalette;
    bluePalette.setColor(QPalette::Window,
                         QColor(blueWindowR, blueWindowG, blueWindowB));
    bluePalette.setColor(QPalette::WindowText, Qt::black);
    bluePalette.setColor(QPalette::Base, Qt::white);
    // Text roles must be set explicitly: unset roles fall back to the system
    // default palette, which is light on a dark-default desktop and leaves
    // line edits and item views (tree/table) unreadable on the white Base.
    bluePalette.setColor(QPalette::Text, Qt::black);
    bluePalette.setColor(QPalette::AlternateBase,
                         QColor(blueWindowR, blueWindowG, blueWindowB));
    bluePalette.setColor(QPalette::ToolTipBase, Qt::white);
    bluePalette.setColor(QPalette::ToolTipText, Qt::black);
    bluePalette.setColor(QPalette::HighlightedText, Qt::white);
    bluePalette.setColor(QPalette::Button,
                         QColor(blueButtonR, blueButtonG, blueButtonB));
    bluePalette.setColor(QPalette::ButtonText, Qt::black);
    // Demo Blue ships its own icon set (white glyphs on a blue badge) so it is
    // easily told apart from aIDE's built-in aide-dark / aide-light icons. The
    // theme name must match the directory under the search path (see
    // demo_icons.qrc: :/demo/icons/demo-blue/index.theme).
    app.appearanceManager().registerTheme(
        {.name            = "Demo Blue",
         .palette         = bluePalette,
         .iconThemeName   = "demo-blue",
         .iconSearchPaths = {":/demo/icons"}});

    // Add icon search path for the System theme based on current color scheme
    app.appearanceManager().addIconSearchPath(
        "System",
        demo::iconPathForScheme(app.appearanceManager().colorScheme()));

    // Update demo icon search path when the OS color scheme changes. A
    // string-based connection to a QObject slot is used on purpose; see
    // demo::ColorSchemeReactor for the rationale.
    demo::ColorSchemeReactor colorSchemeReactor(app.appearanceManager());
    QObject::connect(
        &app.appearanceManager(), SIGNAL(colorSchemeChanged(aide::ColorScheme)),
        &colorSchemeReactor, SLOT(onColorSchemeChanged(aide::ColorScheme)));

    auto mainWindow           = app.mainWindow();
    auto& notificationManager = app.notificationManager();

    auto* description(new QLabel(
        QApplication::translate("",
                                "This text uses the translation system of aIDE "
                                "library within the demo application"),
        mainWindow.get()));

    // Notification log view on the right: aIDE-owned (#155) and already
    // wired so its "Notification Settings..." entries open Settings on the
    // Notifications page. The demo only decides where to put it.
    auto demoSettings = aide::AideSettingsProvider::unversionableSettings();
    auto* notificationView = mainWindow->notificationLogView();

    auto* centralSplitter = new QSplitter(Qt::Horizontal, mainWindow.get());
    centralSplitter->addWidget(description);
    centralSplitter->addWidget(notificationView);
    mainWindow->setCentralWidget(centralSplitter);

    app.settingsPageRegistry().addPage(std::make_unique<DemoSettingsPage>(
        aide::HierarchicalId("Demo Page 1")("Demo Subpage 1")(
            "Demo Subpage 1.1"),
        aide::AideSettingsProvider::unversionableSettings()));
    app.settingsPageRegistry().addPage(std::make_unique<DemoSettingsPage>(
        aide::HierarchicalId("Demo Page 1")("Demo Subpage 1")(
            "Demo Subpage 1.2"),
        aide::AideSettingsProvider::unversionableSettings()));

    // extend file menu

    auto actionRegistry{app.actionRegistry()};

    auto menuFileContainer{actionRegistry->getMenuContainer(
        aide::HierarchicalId("Main Menu")("File"))};
    auto actionNewProject =
        std::make_shared<QAction>(QApplication::tr("New project ..."));

    if (menuFileContainer.has_value()) {
        auto* menuFile{menuFileContainer.value()->menu()};
        actionNewProject->setParent(menuFile);
        QObject::connect(
            actionNewProject.get(), &QAction::triggered, [mainWindow]() {
                QMessageBox::information(
                    mainWindow.get(), QApplication::tr("Menu Extension"),
                    QApplication::tr("This action shows the menu extension "
                                     "capabilities of aIDE."));
            });
        // clang-format off
        menuFile->insertAction(
            actionRegistry->action(CONSTANTS().FILE_SETTINGS).value(), /* NOLINT(bugprone-unchecked-optional-access) */
            actionNewProject.get());
        menuFile->insertSeparator(
            actionRegistry->action(CONSTANTS().FILE_SETTINGS).value()); /* NOLINT(bugprone-unchecked-optional-access) */
        // clang-format on
        mainWindow->menuBar()->update();

        actionRegistry->registerAction(
            actionNewProject, aide::HierarchicalId("Main Menu")("File")("New"));
    }

    // Checkable "Notification Log" toggle in the View menu (created by aIDE
    // itself since ViewFullscreenAction defaults on and the demo does not
    // disable it). Visibility and splitter width persist across restarts via
    // the demo's unversionable settings.
    auto menuViewContainer{
        actionRegistry->getMenuContainer(CONSTANTS().MENU_VIEW)};

    // Declared outside the if-block: ActionRegistry::registerAction() only
    // keeps a weak_ptr, and QAction::setParent() is QObject ownership, not
    // shared_ptr ownership, so a shared_ptr scoped to the if-block would drop
    // to zero refs and delete the action (silently removing it from the
    // menu) before the window is ever painted.
    std::shared_ptr<QAction> actionNotificationLog;

    if (menuViewContainer.has_value()) {
        auto* menuView{menuViewContainer.value()->menu()};

        const auto notificationLogVisibleKey{
            aide::HierarchicalId("Demo")("NotificationLog")("Visible")};
        const auto notificationLogSplitterKey{
            aide::HierarchicalId("Demo")("NotificationLog")("SplitterState")};

        const bool notificationLogVisible =
            demoSettings->value(notificationLogVisibleKey, true).toBool();
        notificationView->setVisible(notificationLogVisible);

        if (const auto splitterState =
                demoSettings->value(notificationLogSplitterKey).toByteArray();
            !splitterState.isEmpty()) {
            centralSplitter->restoreState(splitterState);
        }

        actionNotificationLog =
            std::make_shared<QAction>(QApplication::tr("Notification Log"));
        actionNotificationLog->setCheckable(true);
        actionNotificationLog->setChecked(notificationLogVisible);
        actionNotificationLog->setParent(menuView);
        menuView->addAction(actionNotificationLog.get());
        mainWindow->menuBar()->update();
        actionRegistry->registerAction(
            actionNotificationLog,
            aide::HierarchicalId("Main Menu")("View")("Notification Log"));

        QObject::connect(
            actionNotificationLog.get(), &QAction::toggled, notificationView,
            [notificationView, demoSettings,
             notificationLogVisibleKey](bool checked) {
                notificationView->setVisible(checked);
                demoSettings->setValue(notificationLogVisibleKey, checked);
                demoSettings->save();
            });

        QObject::connect(
            centralSplitter, &QSplitter::splitterMoved, centralSplitter,
            [centralSplitter, demoSettings, notificationLogSplitterKey]() {
                demoSettings->setValue(notificationLogSplitterKey,
                                       centralSplitter->saveState());
                demoSettings->save();
            });
    }

    // "Demo" menu (placed before "Help") with the Notifications launcher
    // (#164). Two demo NotificationGroups are registered once at startup and
    // shared with every launcher dialog instance the action opens.
    const auto demoBalloonGroupId{aide::HierarchicalId("Demo")("Balloon")};
    const auto demoStickyBalloonGroupId{
        aide::HierarchicalId("Demo")("Sticky Balloon")};

    notificationManager.registerGroup(
        {.id                 = demoBalloonGroupId,
         .displayName        = QApplication::tr("Demo Balloon"),
         .defaultDisplayType = aide::NotificationDisplayType::Balloon});
    notificationManager.registerGroup(
        {.id                 = demoStickyBalloonGroupId,
         .displayName        = QApplication::tr("Demo Sticky Balloon"),
         .defaultDisplayType = aide::NotificationDisplayType::StickyBalloon});

    auto* menuDemoContainer{actionRegistry->createMenu(
        aide::HierarchicalId("Main Menu")("Demo"), mainWindow->menuBar())};
    auto* menuDemo{menuDemoContainer->menu()};
    menuDemo->setTitle(QApplication::tr("&Demo"));

    auto actionNotifications =
        std::make_shared<QAction>(QApplication::tr("Notifications"));
    actionNotifications->setParent(menuDemo);
    menuDemo->addAction(actionNotifications.get());
    actionRegistry->registerAction(
        actionNotifications,
        aide::HierarchicalId("Main Menu")("Demo")("Notifications"));

    QObject::connect(actionNotifications.get(), &QAction::triggered,
                     [&notificationManager, mainWindow, demoBalloonGroupId,
                      demoStickyBalloonGroupId]() {
                         demo::NotificationLauncherDialog dialog(
                             notificationManager, *mainWindow,
                             demoBalloonGroupId, demoStickyBalloonGroupId,
                             mainWindow.get());
                         dialog.exec();
                     });

    // clang-format off
    mainWindow->menuBar()->insertMenu(
        actionRegistry->getMenuContainer(CONSTANTS().MENU_HELP).value()->menu()->menuAction(), /* NOLINT(bugprone-unchecked-optional-access) */
        menuDemo);
    // clang-format on

    return aide::Application::exec();
}
