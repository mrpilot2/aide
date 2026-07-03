#include <array>

#include <catch2/catch_test_macros.hpp>

#include "menucontainer.hpp"

using aide::MenuContainer;

TEST_CASE("A new menu container")
{
    SECTION(" creates a valid QMenu")
    {
        const MenuContainer container;

        REQUIRE(container.menu() != nullptr);
    }
}
