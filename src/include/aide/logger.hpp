#ifndef AIDE_LOGGER_HPP
#define AIDE_LOGGER_HPP

#include <memory>

#include <spdlog/spdlog.h>
#include <string_view>

namespace aide
{
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
        Logger(const std::string& logFileName);

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
        std::shared_ptr<spdlog::logger> m_logger;
    };
} // namespace aide

#endif // AIDE_LOGGER_HPP
