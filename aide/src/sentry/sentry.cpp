
#include "sentry.hpp"

#include <sentry.h>
#include <thread>

using aide::Sentry;
using namespace std::chrono_literals;

namespace
{
    std::string createRelease(const aide::SentryConfig& config)
    {
        return config.applicationName + "@" + config.applicationVersion;
    }

} // namespace

void Sentry::initializeConnection(const SentryConfig& config)
{
    if (config.dsn.empty()) { return; }
    sentry_options_t* options = sentry_options_new();
    sentry_options_set_dsn(options, config.dsn.c_str());

    sentry_options_set_database_path(options, config.databasePath.c_str());

    sentry_options_set_release(options, ::createRelease(config).c_str());

    sentry_init(options);
}

void Sentry::closeConnection()
{
    sentry_close();
}