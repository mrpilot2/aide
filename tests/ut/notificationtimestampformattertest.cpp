#include <catch2/catch_test_macros.hpp>

#include <QDateTime>

#include "notificationtimestampformatter.hpp"

using aide::widgets::NotificationTimestampFormatter;

TEST_CASE("NotificationTimestampFormatter", "[NotificationTimestampFormatter]")
{
    const QDateTime now{QDate(2026, 7, 19), QTime(17, 34)};

    SECTION("today formats as an absolute clock time")
    {
        const QDateTime timestamp{QDate(2026, 7, 19), QTime(17, 34)};
        REQUIRE(NotificationTimestampFormatter::format(timestamp, now) ==
                "5:34 PM");
    }

    SECTION("yesterday is prefixed")
    {
        const QDateTime timestamp{QDate(2026, 7, 18), QTime(17, 34)};
        REQUIRE(NotificationTimestampFormatter::format(timestamp, now) ==
                "Yesterday 5:34 PM");
    }

    SECTION("older falls back to a bare date")
    {
        const QDateTime timestamp{QDate(2026, 3, 5), QTime(9, 0)};
        REQUIRE(NotificationTimestampFormatter::format(timestamp, now) ==
                "Mar 5");
    }
}
