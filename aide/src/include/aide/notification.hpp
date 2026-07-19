#ifndef AIDE_NOTIFICATION_HPP
#define AIDE_NOTIFICATION_HPP

#include <vector>

#include <QDateTime>
#include <QMetaType>
#include <QString>

#include <aide/hierarchicalid.hpp>
#include <aide/notificationaction.hpp>
#include <aide/notificationid.hpp>
#include <aide/notificationtype.hpp>

namespace aide
{
    // An immutable snapshot once posted. Carries no read/dismissed flag: that
    // lifecycle state lives in the manager, since the log is the canonical
    // source of truth and balloons/banners are ephemeral views of it.
    class Notification
    {
    public:
        HierarchicalId groupId{"notifications"};
        NotificationType type{NotificationType::Information};
        QString title;
        QString content;
        std::vector<NotificationAction> actions;

        // Assigned by the manager on post().
        NotificationId id;
        QDateTime timestamp;
    };
} // namespace aide

// Lets the notification-view model (#155) carry a full Notification through
// QVariant/QModelIndex::data() without a separate role per field.
Q_DECLARE_METATYPE(aide::Notification)

#endif // AIDE_NOTIFICATION_HPP
