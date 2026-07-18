#include <algorithm>

#include <catch2/catch_test_macros.hpp>

#include <QApplication>
#include <QMenu>
#include <QToolButton>

#include <aide/aideconstants.hpp>
#include <aide/applicationconfig.hpp>
#include <aide/notificationtype.hpp>

#include "actionregistry.hpp"
#include "aide/gui/widgets/banner.hpp"
#include "mainwindow.hpp"
#include "mocksettings.hpp"
#include "nulllogger.hpp"

using aide::ActionRegistry;
using aide::ApplicationConfig;
using aide::NotificationType;
using aide::constants::CONSTANTS;
using aide::gui::MainWindow;
using aide::test::MockSettings;
using aide::test::NullLogger;
using aide::widgets::Banner;

TEST_CASE("Any main window")
{
    QApplication::setApplicationName("aide_test");
    QApplication::setOrganizationName("aide_company");

    MockSettings settings;
    const auto registry = std::make_shared<ActionRegistry>(
        settings, std::make_shared<NullLogger>());

    SECTION("registers actions")
    {
        MainWindow mainWindow(std::make_shared<NullLogger>(),
                              ApplicationConfig{}, nullptr);
        mainWindow.setMainWindowController(nullptr, registry);
        REQUIRE(registry->actions().size() >= 2);
    }
}

TEST_CASE("A main window with the full screen feature enabled")
{
    QApplication::setApplicationName("aide_test");
    QApplication::setOrganizationName("aide_company");

    MockSettings settings;
    const auto registry = std::make_shared<ActionRegistry>(
        settings, std::make_shared<NullLogger>());

    MainWindow mainWindow(std::make_shared<NullLogger>(), ApplicationConfig{},
                          nullptr);
    mainWindow.setMainWindowController(nullptr, registry);

    SECTION("registers a checkable full screen action")
    {
        const auto action = registry->action(CONSTANTS().VIEW_FULLSCREEN);

        REQUIRE(action.has_value());
        // NOLINTNEXTLINE(bugprone-unchecked-optional-access)
        REQUIRE(action.value()->isCheckable());
    }

    SECTION("binds the full screen action to F11")
    {
        REQUIRE(registry->actions()
                    .at(CONSTANTS().VIEW_FULLSCREEN)
                    .defaultKeySequences == QList<QKeySequence>({
                                                QKeySequence(Qt::Key_F11),
                                            }));
    }

    SECTION("toggling full screen preserves the maximized state")
    {
        mainWindow.setWindowState(Qt::WindowMaximized);

        const auto action = registry->action(CONSTANTS().VIEW_FULLSCREEN);
        REQUIRE(action.has_value());

        // NOLINTNEXTLINE(bugprone-unchecked-optional-access)
        action.value()->trigger();
        REQUIRE(mainWindow.windowState().testFlag(Qt::WindowFullScreen));
        REQUIRE(mainWindow.windowState().testFlag(Qt::WindowMaximized));

        // NOLINTNEXTLINE(bugprone-unchecked-optional-access)
        action.value()->trigger();
        REQUIRE_FALSE(mainWindow.windowState().testFlag(Qt::WindowFullScreen));
        REQUIRE(mainWindow.windowState().testFlag(Qt::WindowMaximized));
    }
}

TEST_CASE("A main window with the full screen feature disabled")
{
    QApplication::setApplicationName("aide_test");
    QApplication::setOrganizationName("aide_company");

    MockSettings settings;
    const auto registry = std::make_shared<ActionRegistry>(
        settings, std::make_shared<NullLogger>());

    MainWindow mainWindow(
        std::make_shared<NullLogger>(),
        ApplicationConfig{}.setEnabled(
            ApplicationConfig::Feature::ViewFullscreenAction, false),
        nullptr);
    mainWindow.setMainWindowController(nullptr, registry);

    SECTION("does not register the full screen action")
    {
        REQUIRE_FALSE(
            registry->action(CONSTANTS().VIEW_FULLSCREEN).has_value());
    }

    SECTION("does not create the View menu")
    {
        REQUIRE_FALSE(
            registry->getMenuContainer(CONSTANTS().MENU_VIEW).has_value());
    }
}

TEST_CASE("A main window with the show-log-in-file-manager feature enabled")
{
    QApplication::setApplicationName("aide_test");
    QApplication::setOrganizationName("aide_company");

    MockSettings settings;
    const auto registry = std::make_shared<ActionRegistry>(
        settings, std::make_shared<NullLogger>());

    MainWindow mainWindow(
        std::make_shared<NullLogger>(),
        ApplicationConfig{}.setEnabled(
            ApplicationConfig::Feature::ShowLogInFileManagerAction, true),
        nullptr);
    mainWindow.setMainWindowController(nullptr, registry);

    SECTION("registers the show-log-in-file-manager action")
    {
        REQUIRE(registry->action(CONSTANTS().HELP_SHOW_LOG_IN_FILE_MANAGER)
                    .has_value());
    }

    SECTION("does not bind a default key sequence")
    {
        REQUIRE(registry->actions()
                    .at(CONSTANTS().HELP_SHOW_LOG_IN_FILE_MANAGER)
                    .defaultKeySequences.empty());
    }
}

TEST_CASE(
    "A main window with the show-log-in-file-manager feature left at its "
    "default")
{
    QApplication::setApplicationName("aide_test");
    QApplication::setOrganizationName("aide_company");

    MockSettings settings;
    const auto registry = std::make_shared<ActionRegistry>(
        settings, std::make_shared<NullLogger>());

    MainWindow mainWindow(std::make_shared<NullLogger>(), ApplicationConfig{},
                          nullptr);
    mainWindow.setMainWindowController(nullptr, registry);

    SECTION("does not register the show-log-in-file-manager action")
    {
        REQUIRE_FALSE(
            registry->action(CONSTANTS().HELP_SHOW_LOG_IN_FILE_MANAGER)
                .has_value());
    }
}

TEST_CASE("A main window with the report-bug feature enabled")
{
    QApplication::setApplicationName("aide_test");
    QApplication::setOrganizationName("aide_company");

    MockSettings settings;
    const auto registry = std::make_shared<ActionRegistry>(
        settings, std::make_shared<NullLogger>());

    MainWindow mainWindow(
        std::make_shared<NullLogger>(),
        ApplicationConfig{}.setEnabled(
            ApplicationConfig::Feature::ReportBugAction, true),
        nullptr);
    mainWindow.setMainWindowController(nullptr, registry);

    SECTION("registers the report-bug action")
    {
        REQUIRE(registry->action(CONSTANTS().HELP_REPORT_BUG).has_value());
    }

    SECTION("does not bind a default key sequence")
    {
        REQUIRE(registry->actions()
                    .at(CONSTANTS().HELP_REPORT_BUG)
                    .defaultKeySequences.empty());
    }
}

TEST_CASE("A main window with the report-bug feature left at its default")
{
    QApplication::setApplicationName("aide_test");
    QApplication::setOrganizationName("aide_company");

    MockSettings settings;
    const auto registry = std::make_shared<ActionRegistry>(
        settings, std::make_shared<NullLogger>());

    MainWindow mainWindow(std::make_shared<NullLogger>(), ApplicationConfig{},
                          nullptr);
    mainWindow.setMainWindowController(nullptr, registry);

    SECTION("does not register the report-bug action")
    {
        REQUIRE_FALSE(
            registry->action(CONSTANTS().HELP_REPORT_BUG).has_value());
    }
}

TEST_CASE(
    "A main window with neither the show-log-in-file-manager nor the "
    "report-bug feature enabled")
{
    QApplication::setApplicationName("aide_test");
    QApplication::setOrganizationName("aide_company");

    MockSettings settings;
    const auto registry = std::make_shared<ActionRegistry>(
        settings, std::make_shared<NullLogger>());

    MainWindow mainWindow(std::make_shared<NullLogger>(), ApplicationConfig{},
                          nullptr);
    mainWindow.setMainWindowController(nullptr, registry);

    SECTION("leaves no stray separator before the About actions")
    {
        const auto menuHelpContainer =
            registry->getMenuContainer(CONSTANTS().MENU_HELP);
        REQUIRE(menuHelpContainer.has_value());
        // NOLINTNEXTLINE(bugprone-unchecked-optional-access)
        const auto* menuHelp = menuHelpContainer.value()->menu();

        REQUIRE_FALSE(menuHelp->actions().first()->isSeparator());
    }
}

TEST_CASE(
    "A main window with both the show-log-in-file-manager and the "
    "report-bug feature enabled registers both gated actions")
{
    QApplication::setApplicationName("aide_test");
    QApplication::setOrganizationName("aide_company");

    MockSettings settings;
    const auto registry = std::make_shared<ActionRegistry>(
        settings, std::make_shared<NullLogger>());

    MainWindow mainWindow(
        std::make_shared<NullLogger>(),
        ApplicationConfig{}
            .setEnabled(ApplicationConfig::Feature::ShowLogInFileManagerAction,
                        true)
            .setEnabled(ApplicationConfig::Feature::ReportBugAction, true),
        nullptr);
    mainWindow.setMainWindowController(nullptr, registry);

    REQUIRE(registry->action(CONSTANTS().HELP_SHOW_LOG_IN_FILE_MANAGER)
                .has_value());
    REQUIRE(registry->action(CONSTANTS().HELP_REPORT_BUG).has_value());
}

TEST_CASE("A main window's banner mount", "[MainWindow]")
{
    QApplication::setApplicationName("aide_test");
    QApplication::setOrganizationName("aide_company");

    SECTION("addBanner returns a banner mounted under the window")
    {
        MainWindow mainWindow(std::make_shared<NullLogger>(),
                              ApplicationConfig{}, nullptr);

        auto* banner =
            mainWindow.addBanner(NotificationType::Warning, "Disk space low");

        REQUIRE(banner != nullptr);
        REQUIRE(mainWindow.isAncestorOf(banner));
    }

    SECTION("several active banners stack newest at the bottom")
    {
        MainWindow mainWindow(std::make_shared<NullLogger>(),
                              ApplicationConfig{}, nullptr);

        auto* first =
            mainWindow.addBanner(NotificationType::Information, "First");
        auto* second =
            mainWindow.addBanner(NotificationType::Warning, "Second");

        mainWindow.show();
        QApplication::processEvents();

        REQUIRE(first->y() < second->y());
    }

    SECTION("removeBanner removes the banner from the window")
    {
        MainWindow mainWindow(std::make_shared<NullLogger>(),
                              ApplicationConfig{}, nullptr);

        auto* banner = mainWindow.addBanner(NotificationType::Error, "Boom");
        mainWindow.removeBanner(banner);
        QApplication::processEvents();

        REQUIRE(mainWindow.findChildren<Banner*>().isEmpty());
    }

    SECTION("closing a banner via its close button removes it from the window")
    {
        MainWindow mainWindow(std::make_shared<NullLogger>(),
                              ApplicationConfig{}, nullptr);

        auto* banner = mainWindow.addBanner(NotificationType::Success, "Done");
        auto* closeButton = banner->findChild<QToolButton*>();
        REQUIRE(closeButton != nullptr);
        closeButton->click();
        QApplication::processEvents();

        REQUIRE(mainWindow.findChildren<Banner*>().isEmpty());
    }
}

TEST_CASE(
    "A main window with both the show-log-in-file-manager and the "
    "report-bug feature enabled adds exactly one separator after the "
    "gated actions")
{
    QApplication::setApplicationName("aide_test");
    QApplication::setOrganizationName("aide_company");

    MockSettings settings;
    const auto registry = std::make_shared<ActionRegistry>(
        settings, std::make_shared<NullLogger>());

    MainWindow mainWindow(
        std::make_shared<NullLogger>(),
        ApplicationConfig{}
            .setEnabled(ApplicationConfig::Feature::ShowLogInFileManagerAction,
                        true)
            .setEnabled(ApplicationConfig::Feature::ReportBugAction, true),
        nullptr);
    mainWindow.setMainWindowController(nullptr, registry);

    const auto menuHelpContainer =
        registry->getMenuContainer(CONSTANTS().MENU_HELP);
    REQUIRE(menuHelpContainer.has_value());
    // NOLINTNEXTLINE(bugprone-unchecked-optional-access)
    const auto* menuHelp = menuHelpContainer.value()->menu();
    const auto actions   = menuHelp->actions();

    const auto separatorCount =
        std::count_if(actions.cbegin(), actions.cend(),
                      [](const auto* action) { return action->isSeparator(); });

    REQUIRE(separatorCount == 1);
}
