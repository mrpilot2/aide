
#include <array>

#include <catch2/catch.hpp>

#include "application.hpp"
#include "commonsettingskeys.hpp"
#include "mainwindowgeometryandstate.hpp"
#include "mockmainwindowview.hpp"
#include "mocksettings.hpp"

using aide::core::MainWindowGeometryAndState;
using aide::core::settings::KEYS;
using aide::test::MockMainWindowView;
using aide::test::MockSettings;

TEST_CASE("Any main window geometry and state interactor")
{
    // NOLINTNEXTLINE
    std::array<char*, 1> appName{{const_cast<char*>("aide_test")}};
    int numberOfArgs{1};

    aide::Application::setApplicationName("aide_test");
    aide::Application::setOrganizationName("aide_company");

    aide::Application app(numberOfArgs, appName.data());

    auto mainWindowView = std::make_shared<MockMainWindowView>();
    auto settings       = MockSettings();

    MainWindowGeometryAndState interactor(mainWindowView, settings);

    SECTION("saves window state in settings")
    {
        auto state = QByteArray("def");

        interactor.saveGeometryAndState(QByteArray("abc"), state);

        REQUIRE(state ==
                settings.value(KEYS().UI.MAIN_WINDOW_STATE_KEY).toByteArray());
    }

    SECTION("saves window geometry in settings")
    {
        auto geometry = QByteArray("abc");

        interactor.saveGeometryAndState(geometry, QByteArray("def"));

        REQUIRE(
            geometry ==
            settings.value(KEYS().UI.MAIN_WINDOW_GEOMETRY_KEY).toByteArray());
    }

    SECTION("restores window state from settings")
    {
        auto state = QByteArray("def");

        settings.setValue(KEYS().UI.MAIN_WINDOW_STATE_KEY, state);

        interactor.restoreGeometryAndState();

        REQUIRE(mainWindowView->state() == state);
    }

    SECTION("restores window geometry from settings")
    {
        auto geometry = QByteArray("abc");

        settings.setValue(KEYS().UI.MAIN_WINDOW_GEOMETRY_KEY, geometry);

        interactor.restoreGeometryAndState();

        REQUIRE(mainWindowView->geometry() == geometry);
    }

    SECTION("shows window maximized if geometry and state are not saved")
    {
        interactor.restoreGeometryAndState();

        REQUIRE(mainWindowView->isMaximized());
    }
}
