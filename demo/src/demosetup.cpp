#include "demosetup.hpp"

#include <QDir>
#include <QLabel>
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

using aide::constants::CONSTANTS;

namespace demo
{
    void registerDemoTheme(const aide::Application& app)
    {
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
        // Text roles must be set explicitly: unset roles fall back to the
        // system default palette, which is light on a dark-default desktop
        // and leaves line edits and item views (tree/table) unreadable on
        // the white Base.
        bluePalette.setColor(QPalette::Text, Qt::black);
        bluePalette.setColor(QPalette::AlternateBase,
                             QColor(blueWindowR, blueWindowG, blueWindowB));
        bluePalette.setColor(QPalette::ToolTipBase, Qt::white);
        bluePalette.setColor(QPalette::ToolTipText, Qt::black);
        bluePalette.setColor(QPalette::HighlightedText, Qt::white);
        bluePalette.setColor(QPalette::Button,
                             QColor(blueButtonR, blueButtonG, blueButtonB));
        bluePalette.setColor(QPalette::ButtonText, Qt::black);
        // Demo Blue ships its own icon set (white glyphs on a blue badge) so
        // it is easily told apart from aIDE's built-in aide-dark / aide-light
        // icons. The theme name must match the directory under the search
        // path (see demo_icons.qrc: :/demo/icons/demo-blue/index.theme).
        app.appearanceManager().registerTheme(
            {.name            = "Demo Blue",
             .palette         = bluePalette,
             .iconThemeName   = "demo-blue",
             .iconSearchPaths = {":/demo/icons"}});

        // Add icon search path for the System theme based on current color
        // scheme
        app.appearanceManager().addIconSearchPath(
            "System", iconPathForScheme(app.appearanceManager().colorScheme()));
    }

    std::unique_ptr<ColorSchemeReactor> watchColorScheme(
        const aide::Application& app)
    {
        // Update demo icon search path when the OS color scheme changes. A
        // string-based connection to a QObject slot is used on purpose; see
        // demo::ColorSchemeReactor for the rationale.
        auto reactor =
            std::make_unique<ColorSchemeReactor>(app.appearanceManager());
        QObject::connect(&app.appearanceManager(),
                         SIGNAL(colorSchemeChanged(aide::ColorScheme)),
                         reactor.get(),
                         SLOT(onColorSchemeChanged(aide::ColorScheme)));
        return reactor;
    }

    void buildCentralWidget(const aide::Application& app)
    {
        auto mainWindow = app.mainWindow();

        auto* description(new QLabel(
            QApplication::translate(
                "",
                "This text uses the translation system of aIDE library "
                "within the demo application"),
            mainWindow.get()));

        // Notification log view on the right: aIDE-owned (#155) and already
        // wired so its "Notification Settings..." entries open Settings on
        // the Notifications page. The demo only decides where to put it.
        auto demoSettings = aide::AideSettingsProvider::unversionableSettings();
        auto* notificationView = mainWindow->notificationLogView();

        auto* centralSplitter = new QSplitter(Qt::Horizontal, mainWindow.get());
        centralSplitter->addWidget(description);
        centralSplitter->addWidget(notificationView);
        mainWindow->setCentralWidget(centralSplitter);

        if (const auto splitterState =
                demoSettings
                    ->value(aide::HierarchicalId("Demo")("NotificationLog")(
                        "SplitterState"))
                    .toByteArray();
            !splitterState.isEmpty()) {
            centralSplitter->restoreState(splitterState);
        }
    }

    void registerDemoSettingsPages(const aide::Application& app)
    {
        app.settingsPageRegistry().addPage(std::make_unique<DemoSettingsPage>(
            aide::HierarchicalId("Demo Page 1")("Demo Subpage 1")(
                "Demo Subpage 1.1"),
            aide::AideSettingsProvider::unversionableSettings()));
        app.settingsPageRegistry().addPage(std::make_unique<DemoSettingsPage>(
            aide::HierarchicalId("Demo Page 1")("Demo Subpage 1")(
                "Demo Subpage 1.2"),
            aide::AideSettingsProvider::unversionableSettings()));
    }

    std::shared_ptr<QAction> extendFileMenu(const aide::Application& app)
    {
        auto mainWindow     = app.mainWindow();
        auto actionRegistry = app.actionRegistry();
        auto menuFileContainer{actionRegistry->getMenuContainer(
            aide::HierarchicalId("Main Menu")("File"))};

        std::shared_ptr<QAction> actionNewProject;
        if (!menuFileContainer.has_value()) { return actionNewProject; }

        actionNewProject =
            std::make_shared<QAction>(QApplication::tr("New project ..."));

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

        return actionNewProject;
    }

    std::shared_ptr<QAction> addNotificationLogToggle(
        const aide::Application& app)
    {
        auto mainWindow     = app.mainWindow();
        auto actionRegistry = app.actionRegistry();

        // Checkable "Notification Log" toggle in the View menu (created by
        // aIDE itself since ViewFullscreenAction defaults on and the demo
        // does not disable it). Visibility and splitter width persist across
        // restarts via the demo's unversionable settings.
        auto menuViewContainer{
            actionRegistry->getMenuContainer(CONSTANTS().MENU_VIEW)};

        std::shared_ptr<QAction> actionNotificationLog;
        if (!menuViewContainer.has_value()) { return actionNotificationLog; }

        auto* menuView{menuViewContainer.value()->menu()};
        auto* notificationView = mainWindow->notificationLogView();
        auto* centralSplitter =
            qobject_cast<QSplitter*>(mainWindow->centralWidget());
        auto demoSettings = aide::AideSettingsProvider::unversionableSettings();

        const auto notificationLogVisibleKey{
            aide::HierarchicalId("Demo")("NotificationLog")("Visible")};
        const auto notificationLogSplitterKey{
            aide::HierarchicalId("Demo")("NotificationLog")("SplitterState")};

        const bool notificationLogVisible =
            demoSettings->value(notificationLogVisibleKey, true).toBool();
        notificationView->setVisible(notificationLogVisible);

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

        if (centralSplitter != nullptr) {
            QObject::connect(
                centralSplitter, &QSplitter::splitterMoved, centralSplitter,
                [centralSplitter, demoSettings, notificationLogSplitterKey]() {
                    demoSettings->setValue(notificationLogSplitterKey,
                                           centralSplitter->saveState());
                    demoSettings->save();
                });
        }

        return actionNotificationLog;
    }

    std::shared_ptr<QAction> buildDemoMenu(const aide::Application& app)
    {
        auto mainWindow           = app.mainWindow();
        auto actionRegistry       = app.actionRegistry();
        auto& notificationManager = app.notificationManager();

        // "Demo" menu (placed before "Help") with the Notifications launcher
        // (#164). Two demo NotificationGroups are registered once at startup
        // and shared with every launcher dialog instance the action opens.
        const auto demoBalloonGroupId{aide::HierarchicalId("Demo")("Balloon")};
        const auto demoStickyBalloonGroupId{
            aide::HierarchicalId("Demo")("Sticky Balloon")};

        notificationManager.registerGroup(
            {.id                 = demoBalloonGroupId,
             .displayName        = QApplication::tr("Demo Balloon"),
             .defaultDisplayType = aide::NotificationDisplayType::Balloon});
        notificationManager.registerGroup(
            {.id          = demoStickyBalloonGroupId,
             .displayName = QApplication::tr("Demo Sticky Balloon"),
             .defaultDisplayType =
                 aide::NotificationDisplayType::StickyBalloon});

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

        return actionNotifications;
    }
} // namespace demo
