

#ifndef AIDE_LOGGER_FACTORY_HPP
#define AIDE_LOGGER_FACTORY_HPP

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

    private:
        static LoggerPtr setupLogger(const std::string& loggerName);

        static bool tryToCreateLogLocationIfItDoesNotExist(
            const QString& logLocation);
    };
} // namespace aide::core

#endif // AIDE_LOGGER_FACTORY_HPP
