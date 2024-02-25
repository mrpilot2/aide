
#include <catch2/catch_test_macros.hpp>

#include "aidesettingsprovider.hpp"

using aide::AideSettingsProvider;

TEST_CASE("Any AideSettingsProvider")
{
    AideSettingsProvider provider;

    SECTION(" provides valid versionable settings")
    {
        REQUIRE(provider.versionableSettings() != nullptr);
    }

    SECTION(" provides valid unversionable settings")
    {
        REQUIRE(provider.unversionableSettings() != nullptr);
    }
}
