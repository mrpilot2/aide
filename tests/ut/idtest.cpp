
#include "catch.hpp"

#include <aide/id.hpp>
#include <iostream>

using aide::Id;

TEST_CASE("Any ID")
{
    SECTION(" calculates it's hash on construction")
    {
        Id id("This is a constexpr id");

        REQUIRE(Id("This is a constexpr id").value() == 5812189927597425105);
    }

    SECTION(" can be alphabetically compared")
    {
        Id lower("This is the lower one");
        Id higher("This is the more higher one");

        REQUIRE(lower.isAlphabeticallyBefore(higher));
    }

    SECTION(" can be created in a hierarchy")
    {
        Id hierarchy("Aide", "File", "Open");

        REQUIRE(hierarchy.name() == "Aide::File::Open");
    }
}
