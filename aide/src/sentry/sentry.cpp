
#include "aide/sentry.hpp"

#include <sentry.h>

#include <aide/buildinformation.hpp>

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

    sentry_set_tag("qt.version", QT_VERSION_STR);
    sentry_set_tag("aide.version",
                   aide::build_information::AIDE_VERSION_STRING);
}

void Sentry::closeConnection()
{
    sentry_close();
}

void Sentry::captureException(const std::exception& exception)
{
    const sentry_value_t event = sentry_value_new_event();

    const sentry_value_t exc =
        sentry_value_new_exception("Exception", exception.what());
    sentry_value_set_stacktrace(exc, NULL, 0);
    sentry_event_add_exception(event, exc);

    sentry_capture_event(event);
}

void Sentry::captureUnknownException()
{
    const sentry_value_t event = sentry_value_new_event();

    const sentry_value_t exc =
        sentry_value_new_exception("Exception", "Unknown exception");
    sentry_value_set_stacktrace(exc, NULL, 0);
    sentry_event_add_exception(event, exc);

    sentry_capture_event(event);
}
