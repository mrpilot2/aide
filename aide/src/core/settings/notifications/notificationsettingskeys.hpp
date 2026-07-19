#ifndef AIDE_NOTIFICATION_SETTINGS_KEYS_HPP
#define AIDE_NOTIFICATION_SETTINGS_KEYS_HPP

#include <aide/hierarchicalid.hpp>

namespace aide::core
{
    // The locked notification settings keys (#154), shared by
    // NotificationManager, NotificationBalloonHost and
    // NotificationsSettingsPage so the format is defined exactly once.
    constexpr auto* NOTIFICATIONS_SETTINGS_ROOT           = "notifications";
    constexpr auto* NOTIFICATIONS_DO_NOT_DISTURB_KEY      = "doNotDisturb";
    constexpr auto* NOTIFICATIONS_BALLOON_PLACEMENT_KEY   = "balloonPlacement";
    constexpr auto* NOTIFICATIONS_DISPLAY_TYPE_KEY_SUFFIX = "displayType";

    [[nodiscard]] inline HierarchicalId notificationDoNotDisturbKey()
    {
        return HierarchicalId(NOTIFICATIONS_SETTINGS_ROOT)(
            NOTIFICATIONS_DO_NOT_DISTURB_KEY);
    }

    [[nodiscard]] inline HierarchicalId notificationBalloonPlacementKey()
    {
        return HierarchicalId(NOTIFICATIONS_SETTINGS_ROOT)(
            NOTIFICATIONS_BALLOON_PLACEMENT_KEY);
    }

    // notifications.<group-id>.displayType
    [[nodiscard]] inline HierarchicalId notificationDisplayTypeKey(
        const HierarchicalId& groupId)
    {
        auto key = HierarchicalId(NOTIFICATIONS_SETTINGS_ROOT);
        for (const auto* level : groupId) {
            key.addLevel(level);
        }
        key.addLevel(NOTIFICATIONS_DISPLAY_TYPE_KEY_SUFFIX);
        return key;
    }
} // namespace aide::core

#endif // AIDE_NOTIFICATION_SETTINGS_KEYS_HPP
