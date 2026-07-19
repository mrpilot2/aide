#include "notificationtimestampformatter.hpp"

#include <QDateTime>

using aide::widgets::NotificationTimestampFormatter;

namespace
{
    constexpr int YESTERDAY_OFFSET_DAYS{-1};
} // namespace

QString NotificationTimestampFormatter::format(const QDateTime& timestamp,
                                               const QDateTime& now)
{
    const QDate today = now.date();
    const QDate date  = timestamp.date();
    QString time      = timestamp.toString(QStringLiteral("h:mm AP"));

    if (date == today) { return time; }
    if (date == today.addDays(YESTERDAY_OFFSET_DAYS)) {
        return tr("Yesterday %1").arg(time);
    }
    return timestamp.toString(QStringLiteral("MMM d"));
}
