#include <catch2/catch_test_macros.hpp>

#include <aide/settings/settingspageregistry.hpp>

#include "mocksettingspage.hpp"

using aide::core::SettingsPagePtr;
using aide::core::SettingsPageRegistry;
using aide::test::MockSettingsPage;

TEST_CASE("A new settings page registry")
{
    SECTION(" is empty")
    {
        const SettingsPageRegistry registry;
        REQUIRE(registry.settingsPages().empty());
    }
}

TEST_CASE("Any settings page registry")
{
    SettingsPageRegistry registry;

    SECTION(" can store a settings page")
    {
        registry.addPage(std::make_unique<MockSettingsPage>());
        REQUIRE(registry.settingsPages().size() == 1);
    }

    SECTION(" cannot store a settings page with duplicate group")
    {
        registry.addPage(std::make_unique<MockSettingsPage>());

        REQUIRE_THROWS(registry.addPage(std::make_unique<MockSettingsPage>()));
    }
}
