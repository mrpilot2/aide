#include <catch2/catch_test_macros.hpp>

#include <QApplication>

#include <aide/aideconstants.hpp>
#include <aide/applicationconfig.hpp>

#include "actionregistry.hpp"
#include "mainwindow.hpp"
#include "mocksettings.hpp"
#include "nulllogger.hpp"

using aide::ActionRegistry;
using aide::ApplicationConfig;
using aide::constants::CONSTANTS;
using aide::gui::MainWindow;
using aide::test::MockSettings;
using aide::test::NullLogger;

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
