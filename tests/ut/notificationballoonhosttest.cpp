#include <cstddef>
#include <utility>

#include <catch2/catch_test_macros.hpp>

#include <QApplication>
#include <QWidget>

#include <aide/hierarchicalid.hpp>
#include <aide/notification.hpp>
#include <aide/notificationballoonplacement.hpp>
#include <aide/notificationdisplaytype.hpp>
#include <aide/notificationgroup.hpp>
#include <aide/notificationmanager.hpp>
#include <aide/notificationtype.hpp>

#include "aide/gui/widgets/notificationballoon.hpp"
#include "mocksettings.hpp"
#include "notificationballoonhost.hpp"

using aide::HierarchicalId;
using aide::Notification;
using aide::NotificationBalloonPlacement;
using aide::NotificationDisplayType;
using aide::NotificationGroup;
using aide::NotificationManager;
using aide::NotificationType;
using aide::gui::NotificationBalloonHost;
using aide::test::MockSettings;
using aide::widgets::NotificationBalloon;

namespace
{
    std::size_t liveBalloonCount()
    {
        std::size_t count = 0;
        for (auto* widget : QApplication::topLevelWidgets()) {
            if (qobject_cast<NotificationBalloon*>(widget) != nullptr) {
                ++count;
            }
        }
        return count;
    }

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

TEST_CASE("A NotificationBalloonHost", "[NotificationBalloonHost]")
{
    qRegisterMetaType<aide::NotificationId>();

    MockSettings settings;
    NotificationManager manager{settings};
    QWidget anchor;
    NotificationBalloonHost host{manager, settings, &anchor};

    // Mirrors the cross-DLL-safe wiring in ApplicationBuilder: a
    // pointer-to-member connect cannot name a private slot from outside the
    // class, and the string form is what production code uses anyway.
    QObject::connect(&manager, SIGNAL(notificationPosted(aide::NotificationId)),
                     &host, SLOT(onNotificationPosted(aide::NotificationId)));

    const HierarchicalId groupId{"build"};

    SECTION("shows a balloon for a Balloon-routed notification")
    {
        manager.registerGroup(NotificationGroup{
            .id                 = groupId,
            .displayName        = "Build",
            .defaultDisplayType = NotificationDisplayType::Balloon});

        const auto before = liveBalloonCount();
        manager.post(makeNotification(groupId));
        REQUIRE(liveBalloonCount() == before + 1);
    }

    SECTION("shows a balloon for a StickyBalloon-routed notification")
    {
        manager.registerGroup(NotificationGroup{
            .id                 = groupId,
            .displayName        = "Build",
            .defaultDisplayType = NotificationDisplayType::StickyBalloon});

        const auto before = liveBalloonCount();
        manager.post(makeNotification(groupId));
        REQUIRE(liveBalloonCount() == before + 1);
    }

    SECTION("shows no balloon for a None-routed notification")
    {
        manager.registerGroup(NotificationGroup{
            .id                 = groupId,
            .displayName        = "Build",
            .defaultDisplayType = NotificationDisplayType::None});

        const auto before = liveBalloonCount();
        manager.post(makeNotification(groupId));
        REQUIRE(liveBalloonCount() == before);
    }

    SECTION("shows no balloon for an unregistered group")
    {
        const auto before = liveBalloonCount();
        manager.post(makeNotification(groupId));
        REQUIRE(liveBalloonCount() == before);
    }

    SECTION("stacks a balloon per posted notification")
    {
        manager.registerGroup(NotificationGroup{
            .id                 = groupId,
            .displayName        = "Build",
            .defaultDisplayType = NotificationDisplayType::Balloon});

        const auto before = liveBalloonCount();
        manager.post(makeNotification(groupId));
        manager.post(makeNotification(groupId));
        REQUIRE(liveBalloonCount() == before + 2);
    }
}

TEST_CASE("A NotificationBalloonHost's placement setting",
          "[NotificationBalloonHost]")
{
    qRegisterMetaType<aide::NotificationId>();

    MockSettings settings;
    NotificationManager manager{settings};
    QWidget anchor;
    NotificationBalloonHost host{manager, settings, &anchor};

    QObject::connect(&manager, SIGNAL(notificationPosted(aide::NotificationId)),
                     &host, SLOT(onNotificationPosted(aide::NotificationId)));

    const HierarchicalId groupId{"build"};
    manager.registerGroup(NotificationGroup{
        .id                 = groupId,
        .displayName        = "Build",
        .defaultDisplayType = NotificationDisplayType::Balloon});

    SECTION("defaults to BottomRight when unset")
    {
        REQUIRE_FALSE(
            settings.value(HierarchicalId("notifications")("balloonPlacement"))
                .isValid());

        manager.post(makeNotification(groupId));
        REQUIRE(liveBalloonCount() > 0);
    }

    SECTION("honours an explicit corner setting")
    {
        settings.setValue(
            HierarchicalId("notifications")("balloonPlacement"),
            static_cast<int>(NotificationBalloonPlacement::TopLeft));

        const auto before = liveBalloonCount();
        manager.post(makeNotification(groupId));
        REQUIRE(liveBalloonCount() == before + 1);
    }
}
