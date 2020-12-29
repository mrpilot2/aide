#include <array>

#include <catch2/catch.hpp>

#include <QApplication>

#include "actionregistry.hpp"
#include "mainwindow.hpp"
#include "nulllogger.hpp"

using aide::ActionRegistry;
using aide::gui::MainWindow;
using aide::test::NullLogger;

TEST_CASE("Any main window")
{
    // NOLINTNEXTLINE
    std::array<char*, 1> appName{{const_cast<char*>("aide_test")}};
    int numberOfArgs{1};

    QApplication::setApplicationName("aide_test");
    QApplication::setOrganizationName("aide_company");

    QApplication app(numberOfArgs, appName.data());

    auto registry =
        std::make_shared<ActionRegistry>(std::make_shared<NullLogger>());
    MainWindow mainWindow(std::make_shared<NullLogger>(), nullptr);

    SECTION("registers actions")
    {
        mainWindow.setMainWindowController(nullptr, registry);
        REQUIRE(registry->actions().size() >= 2);
    }
}
