#ifndef AIDE_NOTIFICATION_ID_HPP
#define AIDE_NOTIFICATION_ID_HPP

#include <QMetaType>
#include <QtGlobal>

namespace aide
{
    struct NotificationId
    {
        quint64 value{};

        bool operator==(const NotificationId& rhs) const
        {
            return value == rhs.value;
        }

        bool operator!=(const NotificationId& rhs) const
        {
            return !(*this == rhs);
        }

        bool operator<(const NotificationId& rhs) const
        {
            return value < rhs.value;
        }
    };
} // namespace aide

Q_DECLARE_METATYPE(aide::NotificationId)

#endif // AIDE_NOTIFICATION_ID_HPP
