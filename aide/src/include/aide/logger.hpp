#ifndef AIDE_LOGGER_HPP
#define AIDE_LOGGER_HPP

#include <iostream>
#include <memory>

#include <spdlog/spdlog.h>
#include <string_view>

#include "log_helper_macros.hpp"

namespace aide
{
    struct FileName
    {
        explicit FileName(std::string fileName)
            : m_fileName{std::move(fileName)}
        {}

        const std::string& operator()() const { return m_fileName; }

    private:
        const std::string m_fileName;
    };

    struct LoggerName
    {
        explicit LoggerName(std::string loggerName)
            : m_loggerName{std::move(loggerName)}
        {}

        const std::string& operator()() const { return m_loggerName; }

    private:
        const std::string m_loggerName;
    };

    /**
     * @brief Logging framework
     *
     * This class wraps an external logging framework, spdlog at the moment.
     * Depending on the build type (Release, Debug), this class creates
     * multiple log sinks.
     * For Debug the log sinks are console and a log file
     * For Release it's only the log file
     */
    class Logger
    {
    public:
        Logger();
        explicit Logger(const FileName& logFileName);
        explicit Logger(const LoggerName& loggerName);
        explicit Logger(const FileName& logFileName,
                        const LoggerName& loggerName);

        template <typename... Args>
        void trace(std::string_view fmt, const Args&... args)
        {
            m_logger->trace(fmt, args...);
        }

        template <typename... Args>
        void debug(std::string_view fmt, const Args&... args)
        {
            m_logger->debug(fmt, args...);
        }

        template <typename... Args>
        void info(std::string_view fmt, const Args&... args)
        {
            m_logger->info(fmt, args...);
        }

        template <typename... Args>
        void warn(std::string_view fmt, const Args&... args)
        {
            m_logger->warn(fmt, args...);
        }

        template <typename... Args>
        void error(std::string_view fmt, const Args&... args)
        {
            m_logger->error(fmt, args...);
        }

        template <typename... Args>
        void critical(std::string_view fmt, const Args&... args)
        {
            m_logger->critical(fmt, args...);
        }

        void flush();

    private:
        static void registerLogger(
            const std::shared_ptr<spdlog::logger>& logger);

        static std::vector<spdlog::sink_ptr> createSinks(
            std::string logFileName);

        std::shared_ptr<spdlog::logger> m_logger;
        std::shared_ptr<spdlog::logger> m_macroLogger;
    };
} // namespace aide

#endif // AIDE_LOGGER_HPP