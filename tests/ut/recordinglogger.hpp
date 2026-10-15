#ifndef AIDE_RECORDING_LOGGER_HPP
#define AIDE_RECORDING_LOGGER_HPP

#include <string>
#include <vector>

#include <aide/loggerinterface.hpp>

namespace aide::test
{
    // Records every log line with its level, so tests can check what a class
    // under test logged through an injected LoggerPtr without depending on
    // the real spdlog-backed logger.
    class RecordingLogger : public LoggerInterface
    {
    public:
        struct Line
        {
            LogLevel level;
            std::string message;
        };

        [[nodiscard]] const std::vector<Line>& lines() const;

        [[nodiscard]] bool hasWarning() const;

        void flush() override;

        void setLevel(LogLevel level) override;

    private:
        void doLogTrace(std::string_view message) const override;
        void doLogDebug(std::string_view message) const override;
        void doLogInfo(std::string_view message) const override;
        void doLogWarn(std::string_view message) const override;
        void doLogError(std::string_view message) const override;
        void doLogCritical(std::string_view message) const override;

        void record(LogLevel level, std::string_view message) const;

        mutable std::vector<Line> m_lines;
    };
} // namespace aide::test

#endif // AIDE_RECORDING_LOGGER_HPP
