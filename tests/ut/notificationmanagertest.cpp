#include <memory>

#include <catch2/catch_test_macros.hpp>

#include <QSignalSpy>

#include <aide/hierarchicalid.hpp>
#include <aide/notificationmanager.hpp>

#include "mocksettings.hpp"

using aide::HierarchicalId;
using aide::Notification;
using aide::NotificationDisplayType;
using aide::NotificationGroup;
using aide::NotificationManager;
using aide::NotificationType;
using aide::test::MockSettings;

namespace
{
    constexpr quint64 NON_EXISTENT_NOTIFICATION_ID = 999;

    Notification makeNotification(HierarchicalId groupId)
    {
        Notification notification;
        notification.groupId = std::move(groupId);
        notification.type    = NotificationType::Information;
        notification.title   = "Title";
        notification.content = "Content";
        return notification;
    }
} // namespace

TEST_CASE("A new notification manager", "[NotificationManager]")
{
    MockSettings settings;
    const NotificationManager manager{settings};

    SECTION("has no notifications")
    {
        REQUIRE(manager.notifications().empty());
    }

    SECTION("has no unread notifications")
    {
        REQUIRE_FALSE(manager.hasUnread());
    }

    SECTION("is not in Do Not Disturb mode")
    {
        REQUIRE_FALSE(manager.doNotDisturb());
    }

    SECTION("has no group registered")
    {
        REQUIRE_FALSE(manager.group(HierarchicalId("build")).has_value());
    }
}

TEST_CASE("Any notification manager", "[NotificationManager]")
{
    qRegisterMetaType<aide::NotificationId>();

    MockSettings settings;
    NotificationManager manager{settings};

    const HierarchicalId groupId{"build"};

    SECTION("can register a group and look it up")
    {
        manager.registerGroup(NotificationGroup{
            .id                 = groupId,
            .displayName        = "Build",
            .defaultDisplayType = NotificationDisplayType::Balloon});

        const auto group = manager.group(groupId);
        REQUIRE(group.has_value());
        REQUIRE(group->displayName == "Build"); // NOLINT
    }

    SECTION("post assigns a monotonic id and a timestamp")
    {
        const auto firstId  = manager.post(makeNotification(groupId));
        const auto secondId = manager.post(makeNotification(groupId));

        REQUIRE(firstId != secondId);

        const auto first = manager.notification(firstId);
        REQUIRE(first.has_value());
        REQUIRE_FALSE(first->timestamp.isNull()); // NOLINT
    }

    SECTION("post appends to the log")
    {
        manager.post(makeNotification(groupId));
        manager.post(makeNotification(groupId));

        REQUIRE(manager.notifications().size() == 2);
    }

    SECTION("post emits notificationPosted")
    {
        const QSignalSpy spy(&manager,
                             SIGNAL(notificationPosted(aide::NotificationId)));

        manager.post(makeNotification(groupId));

        REQUIRE(spy.count() == 1);
    }

    SECTION("post sets hasUnread")
    {
        const QSignalSpy spy(&manager, SIGNAL(unreadChanged(bool)));

        manager.post(makeNotification(groupId));

        REQUIRE(manager.hasUnread());
        REQUIRE(spy.count() == 1);
    }

    SECTION("global markRead clears hasUnread without touching per-id state")
    {
        const auto id = manager.post(makeNotification(groupId));

        manager.markRead();

        REQUIRE_FALSE(manager.hasUnread());
        REQUIRE_FALSE(manager.isRead(id));
    }

    SECTION("per-id markRead marks only that notification as read")
    {
        const auto id = manager.post(makeNotification(groupId));

        const QSignalSpy spy(&manager,
                             SIGNAL(notificationUpdated(aide::NotificationId)));

        manager.markRead(id);

        REQUIRE(manager.isRead(id));
        REQUIRE(spy.count() == 1);
    }

    SECTION("non existing notifications are not read")
    {
        REQUIRE_FALSE(
            manager.isRead(aide::NotificationId{NON_EXISTENT_NOTIFICATION_ID}));
    }

    SECTION("remove deletes the notification and emits notificationRemoved")
    {
        const auto id = manager.post(makeNotification(groupId));

        const QSignalSpy spy(&manager,
                             SIGNAL(notificationRemoved(aide::NotificationId)));

        manager.remove(id);

        REQUIRE_FALSE(manager.notification(id).has_value());
        REQUIRE(spy.count() == 1);
    }

    SECTION("removing a non existing notification does not emit a signal")
    {
        const QSignalSpy spy(&manager,
                             SIGNAL(notificationRemoved(aide::NotificationId)));

        manager.remove(aide::NotificationId{NON_EXISTENT_NOTIFICATION_ID});

        REQUIRE(spy.count() == 0);
    }

    SECTION("clearAll empties the log and emits notificationsCleared")
    {
        manager.post(makeNotification(groupId));
        manager.post(makeNotification(groupId));

        const QSignalSpy spy(&manager, SIGNAL(notificationsCleared()));

        manager.clearAll();

        REQUIRE(manager.notifications().empty());
        REQUIRE(spy.count() == 1);
    }

    SECTION("resolvedDisplayType falls back to the group default")
    {
        manager.registerGroup(NotificationGroup{
            .id                 = groupId,
            .displayName        = "Build",
            .defaultDisplayType = NotificationDisplayType::StickyBalloon});

        const auto id = manager.post(makeNotification(groupId));

        REQUIRE(manager.resolvedDisplayType(id) ==
                NotificationDisplayType::StickyBalloon);
    }

    SECTION("resolvedDisplayType is None for an unregistered group")
    {
        const auto id = manager.post(makeNotification(groupId));

        REQUIRE(manager.resolvedDisplayType(id) ==
                NotificationDisplayType::None);
    }

    SECTION("resolvedDisplayType honours a per-group settings override")
    {
        manager.registerGroup(NotificationGroup{
            .id                 = groupId,
            .displayName        = "Build",
            .defaultDisplayType = NotificationDisplayType::Balloon});

        settings.setValue(
            HierarchicalId("notifications")("build")("displayType"),
            static_cast<int>(NotificationDisplayType::None));

        const auto id = manager.post(makeNotification(groupId));

        REQUIRE(manager.resolvedDisplayType(id) ==
                NotificationDisplayType::None);
    }

    SECTION("resolvedDisplayType is None while Do Not Disturb is active")
    {
        manager.registerGroup(NotificationGroup{
            .id                 = groupId,
            .displayName        = "Build",
            .defaultDisplayType = NotificationDisplayType::Balloon});

        manager.setDoNotDisturb(true);

        const auto id = manager.post(makeNotification(groupId));

        REQUIRE(manager.resolvedDisplayType(id) ==
                NotificationDisplayType::None);
    }

    SECTION("resolvedDisplayType is frozen at post time")
    {
        manager.registerGroup(NotificationGroup{
            .id                 = groupId,
            .displayName        = "Build",
            .defaultDisplayType = NotificationDisplayType::Balloon});

        const auto id = manager.post(makeNotification(groupId));

        manager.setDoNotDisturb(true);

        REQUIRE(manager.resolvedDisplayType(id) ==
                NotificationDisplayType::Balloon);
    }

    SECTION("doNotDisturb can be toggled")
    {
        manager.setDoNotDisturb(true);
        REQUIRE(manager.doNotDisturb());

        manager.setDoNotDisturb(false);
        REQUIRE_FALSE(manager.doNotDisturb());
    }
}
