#include <memory>

#include <catch2/catch_test_macros.hpp>

#include <QCheckBox>
#include <QComboBox>
#include <QTableView>

#include <aide/hierarchicalid.hpp>
#include <aide/notificationmanager.hpp>

#include "mocksettings.hpp"
#include "settings/notifications/notificationssettingspage.hpp"

using aide::HierarchicalId;
using aide::NotificationDisplayType;
using aide::NotificationGroup;
using aide::NotificationManager;
using aide::gui::NotificationsSettingsPage;
using aide::test::MockSettings;

namespace
{
    void registerBuildGroup(NotificationManager& manager)
    {
        manager.registerGroup(NotificationGroup{
            .id                 = HierarchicalId("build"),
            .displayName        = "Build",
            .defaultDisplayType = NotificationDisplayType::Balloon});
    }
} // namespace

TEST_CASE("NotificationsSettingsPage initial state",
          "[NotificationsSettingsPage]")
{
    MockSettings settings;
    NotificationManager manager{settings};
    registerBuildGroup(manager);

    NotificationsSettingsPage page{manager, settings};

    SECTION("is not modified")
    {
        REQUIRE_FALSE(page.isModified());
    }

    SECTION("Do Not Disturb reflects the manager")
    {
        const auto* checkBox = page.widget()->findChild<QCheckBox*>();
        REQUIRE(checkBox != nullptr);
        REQUIRE_FALSE(checkBox->isChecked());
    }

    SECTION("table has one row per registered group")
    {
        const auto* tableView = page.widget()->findChild<QTableView*>();
        REQUIRE(tableView != nullptr);
        REQUIRE(tableView->model()->rowCount() == 1);
    }
}

TEST_CASE("NotificationsSettingsPage modified detection",
          "[NotificationsSettingsPage]")
{
    MockSettings settings;
    NotificationManager manager{settings};
    registerBuildGroup(manager);

    NotificationsSettingsPage page{manager, settings};

    SECTION("Do Not Disturb toggle marks page modified")
    {
        auto* checkBox = page.widget()->findChild<QCheckBox*>();
        REQUIRE(checkBox != nullptr);
        checkBox->setChecked(true);
        REQUIRE(page.isModified());
    }

    SECTION("placement change marks page modified")
    {
        auto* combo = page.widget()->findChild<QComboBox*>();
        REQUIRE(combo != nullptr);
        combo->setCurrentIndex(combo->currentIndex() == 0 ? 1 : 0); // NOLINT
        REQUIRE(page.isModified());
    }
}

TEST_CASE("NotificationsSettingsPage apply", "[NotificationsSettingsPage]")
{
    MockSettings settings;
    NotificationManager manager{settings};
    registerBuildGroup(manager);

    NotificationsSettingsPage page{manager, settings};

    SECTION("persists Do Not Disturb to the manager")
    {
        auto* checkBox = page.widget()->findChild<QCheckBox*>();
        REQUIRE(checkBox != nullptr);
        checkBox->setChecked(true);

        page.apply();

        REQUIRE(manager.doNotDisturb());
        REQUIRE_FALSE(page.isModified());
    }
}

TEST_CASE("NotificationsSettingsPage reset", "[NotificationsSettingsPage]")
{
    MockSettings settings;
    NotificationManager manager{settings};
    registerBuildGroup(manager);

    NotificationsSettingsPage page{manager, settings};

    SECTION("discards unapplied changes")
    {
        auto* checkBox = page.widget()->findChild<QCheckBox*>();
        REQUIRE(checkBox != nullptr);
        checkBox->setChecked(true);
        REQUIRE(page.isModified());

        page.reset();

        REQUIRE_FALSE(page.isModified());
        REQUIRE_FALSE(checkBox->isChecked());
    }
}

TEST_CASE("NotificationsSettingsPage search", "[NotificationsSettingsPage]")
{
    MockSettings settings;
    NotificationManager manager{settings};
    registerBuildGroup(manager);

    NotificationsSettingsPage page{manager, settings};

    SECTION("matches a registered group's id")
    {
        REQUIRE(page.matches("build"));
    }

    SECTION("matches a registered group's display name")
    {
        REQUIRE(page.matches("Build"));
    }

    SECTION("does not match an unrelated pattern")
    {
        REQUIRE_FALSE(page.matches("nonexistent"));
    }
}
