#ifndef AIDE_LOGGER_HPP
#define AIDE_LOGGER_HPP

#include <memory>

#include <aide/log_helper_macros.hpp>
#include <aide/loggerinterface.hpp>
#include <spdlog/spdlog.h>
#include <string_view>

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
    class Logger : public LoggerInterface
    {
    public:
        Logger();
        explicit Logger(const FileName& fileName);
        explicit Logger(const LoggerName& loggerName);
        explicit Logger(const FileName& logFileName,
                        const LoggerName& loggerName);

        void flush() override;

    private:
        void doLogTrace(std::string_view message) override;

        void doLogDebug(std::string_view message) override;

        void doLogInfo(std::string_view message) override;
        void doLogWarn(std::string_view message) override;
        void doLogError(std::string_view message) override;
        void doLogCritical(std::string_view message) override;

        static void registerLogger(
            const std::shared_ptr<spdlog::logger>& logger);

        static std::vector<spdlog::sink_ptr> createSinks(
            std::string logFileName);

        std::shared_ptr<spdlog::logger> m_logger;
        std::shared_ptr<spdlog::logger> m_macroLogger;
    };
} // namespace aide

#endif // AIDE_LOGGER_HPP
