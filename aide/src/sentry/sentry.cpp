
#include "aide/sentry.hpp"

#include <sentry.h>

#include <QProcess>

#include <aide/buildinformation.hpp>

using aide::Sentry;
using namespace std::chrono_literals;

namespace
{
    std::string createRelease(const aide::SentryConfig& config)
    {
        return config.applicationName + "@" + config.applicationVersion;
    }

    std::string getUserName()
    {
        QString name = qgetenv("USER");
        if (name.isEmpty()) { name = qgetenv("USERNAME"); }
        return name.toStdString();
    }

    std::string getUserId()
    {
        QProcess process;
        process.setProgram("id");
        process.setArguments({"-u"});
        process.start();
        while (process.state() != QProcess::NotRunning)
            qApp->processEvents();
        return process.readAll().toStdString();
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

    sentry_value_t user = sentry_value_new_object();
    sentry_value_set_by_key(user, "ip_address",
                            sentry_value_new_string("{{auto}}"));
    sentry_value_set_by_key(user, "username",
                            sentry_value_new_string(::getUserName().c_str()));
    sentry_value_set_by_key(user, "id",
                            sentry_value_new_string(::getUserId().c_str()));

    sentry_set_user(user);

    sentry_set_tag("qt.version", QT_VERSION_STR);
    sentry_set_tag("aide.version",
                   aide::build_information::AIDE_VERSION_STRING);

    sentry_value_t event = sentry_value_new_message_event(
        /*   level */ SENTRY_LEVEL_INFO,
        /*  logger */ "custom",
        /* message */ "It works!");

    sentry_capture_event(event);
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
