
#include <catch2/catch.hpp>

#include <aide/aidesettingsprovider.hpp>

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