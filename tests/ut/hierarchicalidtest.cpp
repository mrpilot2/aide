#include <catch2/catch.hpp>

#include "hierarchicalid.hpp"

using aide::HierarchicalId;

TEST_CASE("Hierarchical Id ")
{
    SECTION("can be constructed with one addLevel")
    {
        HierarchicalId id = HierarchicalId("Main Menu").addLevel("File");

        REQUIRE("Main Menu/File" == id.name());
    }

    SECTION("can be constructed with operator ()")
    {
        const auto id{HierarchicalId("Main Menu")("File")("Quit")};

        REQUIRE("Main Menu/File/Quit" == id.name());
    }
}