

#ifndef AIDE_APPLICATION_CONFIG_HPP
#define AIDE_APPLICATION_CONFIG_HPP

#include <string>

#include <QCoreApplication>

namespace aide
{
    struct SentryConfig
    {
        std::string dsn;

        std::string applicationName{
            QCoreApplication::applicationName().toStdString()};

        std::string applicationVersion{
            QCoreApplication::applicationVersion().toStdString()};

        std::string databasePath{createDefaultDatabasePath()};

    private:
        static std::string createDefaultDatabasePath();
    };

    struct ApplicationConfig
    {
        SentryConfig sentry;
    };
} // namespace aide
#endif // AIDE_APPLICATION_CONFIG_HPP
