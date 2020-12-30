#include <catch2/catch.hpp>

#include <aide/hierarchicalid.hpp>

#include "mocksettingspage.hpp"

using aide::HierarchicalId;
using aide::test::MockSettingsPage;

TEST_CASE("Any settings page")
{
    SECTION("stores the group it belongs to")
    {
        const auto expectedId{HierarchicalId("Test")("Mock")};

        MockSettingsPage page;

        REQUIRE(expectedId == page.group());
    }
}
