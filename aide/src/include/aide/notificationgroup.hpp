#ifndef AIDE_NOTIFICATION_GROUP_HPP
#define AIDE_NOTIFICATION_GROUP_HPP

#include <QString>

#include <aide/hierarchicalid.hpp>
#include <aide/notificationdisplaytype.hpp>

namespace aide
{
    // The user-config unit: one row in the notification settings page (#148),
    // one entry in the routing policy (#145).
    class NotificationGroup
    {
    public:
        HierarchicalId id{"notifications"};
        QString displayName;
        NotificationDisplayType defaultDisplayType{
            NotificationDisplayType::None};
        bool logByDefault{false};
    };
} // namespace aide

#endif // AIDE_NOTIFICATION_GROUP_HPP
