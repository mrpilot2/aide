
#include "applicationconfig.hpp"

#include <iostream>

#include <QDir>
#include <QStandardPaths>

using aide::SentryConfig;

namespace
{
    bool tryToCreateDatabaseLocationIfItDoesNotExist(const QString& location)
    {
        if (!location.isEmpty()) {
            const QDir directory;
            if (!directory.mkpath(location)) {
                std::cerr
                    << "Sentry: Could not create database directory: "
                    << location.toStdString()
                    << ". Trying to log into temp directory instead. If this "
                       "also does not work sentry might not be able to log all "
                       "event.";
                return false;
            }
            return true;
        }
        return false;
    }
} // namespace

std::string SentryConfig::createDefaultDatabasePath()
{
    QString databaseLocation(
        QStandardPaths::writableLocation(QStandardPaths::CacheLocation));

    bool success =
        ::tryToCreateDatabaseLocationIfItDoesNotExist(databaseLocation);

    if (!success) {
        databaseLocation =
            QStandardPaths::writableLocation(QStandardPaths::TempLocation);
        success =
            ::tryToCreateDatabaseLocationIfItDoesNotExist(databaseLocation);
    }

    if (success) {
        const QString databasePath(
            databaseLocation.append("/").append(".sentry-native"));

        return databasePath.toStdString();
    }
    return {};
}
