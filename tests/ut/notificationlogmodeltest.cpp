#include <catch2/catch_test_macros.hpp>

#include <QSignalSpy>

#include <aide/hierarchicalid.hpp>
#include <aide/notificationmanager.hpp>

#include "mocksettings.hpp"
#include "notifications/notificationlogmodel.hpp"

using aide::HierarchicalId;
using aide::Notification;
using aide::NotificationManager;
using aide::NotificationType;
using aide::core::NotificationLogModel;
using aide::test::MockSettings;

namespace
{
    Notification makeNotification(HierarchicalId groupId, QString title)
    {
        Notification notification;
        notification.groupId = std::move(groupId);
        notification.type    = NotificationType::Information;
        notification.title   = std::move(title);
        notification.content = "Content";
        return notification;
    }
} // namespace

TEST_CASE("An empty NotificationLogModel", "[NotificationLogModel]")
{
    MockSettings settings;
    NotificationManager manager{settings};

    const NotificationLogModel model{manager};

    REQUIRE(model.rowCount() == 0);
}

TEST_CASE("NotificationLogModel seeded from an existing log",
          "[NotificationLogModel]")
{
    MockSettings settings;
    NotificationManager manager{settings};
    const HierarchicalId groupId{"build"};

    manager.post(makeNotification(groupId, "First"));
    manager.post(makeNotification(groupId, "Second"));

    const NotificationLogModel model{manager};

    REQUIRE(model.rowCount() == 2);
    // newest-first
    REQUIRE(model.notificationAt(0).title == "Second");
    REQUIRE(model.notificationAt(1).title == "First");
}

TEST_CASE("A live NotificationLogModel", "[NotificationLogModel]")
{
    qRegisterMetaType<aide::NotificationId>();

    MockSettings settings;
    NotificationManager manager{settings};
    const HierarchicalId groupId{"build"};

    NotificationLogModel model{manager};

    SECTION("posting inserts the new entry at row 0")
    {
        manager.post(makeNotification(groupId, "First"));

        const QSignalSpy spy(&model, &NotificationLogModel::rowsInserted);

        manager.post(makeNotification(groupId, "Second"));

        REQUIRE(spy.count() == 1);
        REQUIRE(model.rowCount() == 2);
        REQUIRE(model.notificationAt(0).title == "Second");
        REQUIRE(model.notificationAt(1).title == "First");
    }

    SECTION("removing a notification erases its row")
    {
        const auto firstId = manager.post(makeNotification(groupId, "First"));
        manager.post(makeNotification(groupId, "Second"));

        const QSignalSpy spy(&model, &NotificationLogModel::rowsRemoved);

        manager.remove(firstId);

        REQUIRE(spy.count() == 1);
        REQUIRE(model.rowCount() == 1);
        REQUIRE(model.notificationAt(0).title == "Second");
    }

    SECTION("clearAll resets the model to empty")
    {
        manager.post(makeNotification(groupId, "First"));
        manager.post(makeNotification(groupId, "Second"));

        const QSignalSpy spy(&model, &NotificationLogModel::modelReset);

        manager.clearAll();

        REQUIRE(spy.count() == 1);
        REQUIRE(model.rowCount() == 0);
    }

    SECTION("per-id markRead emits dataChanged for that row")
    {
        const auto id = manager.post(makeNotification(groupId, "First"));

        const QSignalSpy spy(&model, &NotificationLogModel::dataChanged);

        manager.markRead(id);

        REQUIRE(spy.count() == 1);
    }
}
