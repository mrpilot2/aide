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

    SECTION("can be extended")
    {
        const auto mainMenu(HierarchicalId("Main Menu"));
        const auto id{mainMenu("File")("Quit")};

        REQUIRE("Main Menu/File/Quit" == id.name());
    }

    SECTION("can be compared with equal operator")
    {
        const auto id1{HierarchicalId("Main Menu")("File")("New")};
        const auto id2{HierarchicalId("Main Menu")("File")("New")};

        REQUIRE(id1 == id2);
    }

    SECTION("can be compared with not operator")
    {
        const auto id1{HierarchicalId("Main Menu")("File")("New")};
        const auto id2{HierarchicalId("Main Menu")("File")("Newa")};

        REQUIRE(id1 != id2);
    }

    SECTION("can be compared with less than operator")
    {
        const auto id1{HierarchicalId("Main Menu")("File")("New")};
        const auto id2{HierarchicalId("Main Menu")("File")("Quit")};

        REQUIRE(id1 < id2);
    }

    SECTION("can be compared with greater than operator")
    {
        const auto id1{HierarchicalId("Main Menu")("File")("Quit")};
        const auto id2{HierarchicalId("Main Menu")("File")("New")};

        REQUIRE(id1 > id2);
    }

    SECTION("can be iterated")
    {
        const auto id{HierarchicalId("Main Menu")("File")("Quit")};

        const auto first = id.begin();
        REQUIRE(std::string(*first) == "Main Menu");
        REQUIRE(std::string(*(first + 1)) == "File");
        REQUIRE(std::string(*(first + 2)) == "Quit");
        REQUIRE(*(first + 2) == *(id.end() - 1));
    }
}