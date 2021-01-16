#include <catch2/catch.hpp>

#include <aide/settings/settingspageregistry.hpp>

#include "mocksettingspage.hpp"

using aide::core::SettingsPagePtr;
using aide::core::SettingsPageRegistry;
using aide::test::MockSettingsPage;

TEST_CASE("A new settings page registry")
{
    SECTION(" is empty")
    {
        REQUIRE(SettingsPageRegistry::settingsPages().empty());
    }
}

TEST_CASE("Any settings page registry")
{
    SettingsPageRegistry::deleteAllPages();

    SECTION(" can store a settings page")
    {
        SettingsPageRegistry::addPage(std::make_unique<MockSettingsPage>());
        REQUIRE(SettingsPageRegistry::settingsPages().size() == 1);
    }

    SECTION(" cannot store a settings page with duplicate group")
    {
        SettingsPageRegistry::addPage(std::make_unique<MockSettingsPage>());

        REQUIRE_THROWS(SettingsPageRegistry::addPage(
            std::make_unique<MockSettingsPage>()));
    }
}
