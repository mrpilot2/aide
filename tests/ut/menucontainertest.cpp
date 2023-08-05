#include <array>

#include <catch2/catch.hpp>

#include <QApplication>

#include "menucontainer.hpp"

using aide::MenuContainer;

TEST_CASE("A new menu container")
{
    int numberOfArgs{1};
    // NOLINTNEXTLINE
    std::array<char*, 1> appName{{const_cast<char*>("aide_test")}};

    QApplication app(numberOfArgs, appName.data());

    SECTION(" creates a valid QMenu")
    {
        MenuContainer const container;

        REQUIRE(container.menu() != nullptr);
    }
}
