#ifndef AIDE_NOTIFICATION_MANAGER_INTERFACE_HPP
#define AIDE_NOTIFICATION_MANAGER_INTERFACE_HPP

#include <memory>
#include <optional>
#include <vector>

#include <aide/notification.hpp>
#include <aide/notificationdisplaytype.hpp>
#include <aide/notificationgroup.hpp>
#include <aide/notificationid.hpp>

namespace aide
{
    class HierarchicalId;

    class NotificationManagerInterface
    {
    public:
        virtual ~NotificationManagerInterface() = default;

        // groups
        virtual void registerGroup(NotificationGroup group) = 0;

        [[nodiscard]] virtual std::optional<NotificationGroup> group(
            const HierarchicalId& id) const = 0;

        // all registered groups, in id order (settings page, #154)
        [[nodiscard]] virtual std::vector<NotificationGroup> groups() const = 0;

        // emit
        virtual NotificationId post(Notification notification) = 0;

        virtual void remove(NotificationId id) = 0;

        virtual void markRead(NotificationId id) = 0;

        virtual void clearAll() = 0;

        // query -- the log, for the notification view
        [[nodiscard]] virtual std::vector<Notification> notifications()
            const = 0;

        [[nodiscard]] virtual std::optional<Notification> notification(
            NotificationId id) const = 0;

        [[nodiscard]] virtual bool isRead(NotificationId id) const = 0;

        // routing (#145): the display type resolved and frozen at post() time
        [[nodiscard]] virtual NotificationDisplayType resolvedDisplayType(
            NotificationId id) const = 0;

        [[nodiscard]] virtual bool doNotDisturb() const = 0;

        virtual void setDoNotDisturb(bool enabled) = 0;

        // boolean unread state for the notification-view opener button
        // (amendment from #148): no count, independent of per-id isRead().
        [[nodiscard]] virtual bool hasUnread() const = 0;

        virtual void markRead() = 0;
    };

    using NotificationManagerInterfacePtr =
        std::shared_ptr<NotificationManagerInterface>;
} // namespace aide

#endif // AIDE_NOTIFICATION_MANAGER_INTERFACE_HPP
