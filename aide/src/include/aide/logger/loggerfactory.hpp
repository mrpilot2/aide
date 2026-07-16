

#ifndef AIDE_LOGGER_FACTORY_HPP
#define AIDE_LOGGER_FACTORY_HPP

#include <map>
#include <optional>
#include <string>

#include <aide/loggerinterface.hpp>

class QString;

namespace aide::core
{
    class LoggerFactory
    {
    public:
        static LoggerPtr createLogger();

        static LoggerPtr createLogger(const std::string& loggerName);

        // Returns the resolved, absolute log file path currently in use, or
        // an absent value if no writable location could be resolved.
        static std::optional<std::string> logFilePath();

    private:
        static LoggerPtr setupLogger(const std::string& loggerName);

        // Shared cache-location-then-temp-location fallback resolution,
        // used by both logger construction and logFilePath() so they can
        // never disagree.
        static std::optional<std::string> resolveLogFilePath();

        static bool tryToCreateLogLocationIfItDoesNotExist(
            const QString& logLocation);

        static std::map<std::string, LoggerPtr>& getLoggers();
    };
} // namespace aide::core

#endif // AIDE_LOGGER_FACTORY_HPP
