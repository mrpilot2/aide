
#include <catch2/catch.hpp>

#include "application.hpp"
#include "hierarchicalid.hpp"
#include "mainwindowgeometryandstate.hpp"
#include "mockmainwindowview.hpp"
#include "mocksettings.hpp"

using aide::HierarchicalId;
using aide::core::MainWindowGeometryAndState;
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
        auto stateGroup = HierarchicalId("State");
        auto state      = QByteArray("def");

        interactor.saveGeometryAndState(QByteArray("abc"), state);

        REQUIRE(state ==
                settings.value(stateGroup, "MainWindow").toByteArray());
    }

    SECTION("saves window geometry in settings")
    {
        auto geometryGroup = HierarchicalId("Geometry");
        auto geometry      = QByteArray("abc");

        interactor.saveGeometryAndState(geometry, QByteArray("def"));

        REQUIRE(geometry ==
                settings.value(geometryGroup, "MainWindow").toByteArray());
    }

    SECTION("restores window state from settings")
    {
        auto stateGroup = HierarchicalId("State");
        auto state      = QByteArray("def");

        settings.setValue(stateGroup, "MainWindow", state);

        interactor.restoreGeometryAndState();

        REQUIRE(mainWindowView->state() == state);
    }

    SECTION("restores window geometry from settings")
    {
        auto geometryGroup = HierarchicalId("Geometry");
        auto geometry      = QByteArray("abc");

        settings.setValue(geometryGroup, "MainWindow", geometry);

        interactor.restoreGeometryAndState();

        REQUIRE(mainWindowView->geometry() == geometry);
    }

    SECTION("shows window maximized if geometry and state are not saved")
    {
        interactor.restoreGeometryAndState();

        REQUIRE(mainWindowView->isMaximized());
    }
}
