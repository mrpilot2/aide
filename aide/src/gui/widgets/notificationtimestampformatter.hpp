#ifndef AIDE_NOTIFICATION_TIMESTAMP_FORMATTER_HPP
#define AIDE_NOTIFICATION_TIMESTAMP_FORMATTER_HPP

#include <QObject>
#include <QString>

class QDateTime;

namespace aide::widgets
{
    // Absolute-clock formatting for the notification-view card timestamp
    // (#155): "5:34 PM" for today, "Yesterday 5:34 PM" for yesterday, else
    // "MMM d". A dedicated QObject subclass purely to give the "Yesterday"
    // string a stable tr() translation context.
    class NotificationTimestampFormatter : public QObject
    {
        Q_OBJECT

    public:
        [[nodiscard]] static QString format(const QDateTime& timestamp,
                                            const QDateTime& now);
    };
} // namespace aide::widgets

#endif // AIDE_NOTIFICATION_TIMESTAMP_FORMATTER_HPP
