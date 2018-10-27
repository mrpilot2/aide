
#include "catch.hpp"

#include <aide/id.hpp>

using aide::Id;

TEST_CASE("Any ID")
{
    SECTION(" is created as constexpr if possible")
    {
        constexpr Id id("This is a constexpr id");
        static_assert(id.value() == 5812189927597425105,
                      "Hashed ID comparison failed");

        REQUIRE(Id("This is a constexpr id").value() == 5812189927597425105);
    }
}
