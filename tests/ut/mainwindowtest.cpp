#include <array>

#include <catch2/catch.hpp>

#include "actionregistry.hpp"
#include "application.hpp"
#include "mainwindow.hpp"

using aide::ActionRegistry;
using aide::gui::MainWindow;

TEST_CASE("Any main window")
{
    // NOLINTNEXTLINE
    std::array<char*, 1> appName{{const_cast<char*>("aide_test")}};
    int numberOfArgs{1};

    aide::Application::setApplicationName("aide_test");
    aide::Application::setOrganizationName("aide_company");

    aide::Application app(numberOfArgs, appName.data());

    auto registry = std::make_shared<ActionRegistry>();
    MainWindow mainWindow(registry);

    SECTION("registers actions on construction")
    {
        REQUIRE(registry->actions().size() >= 2);
    }
}
