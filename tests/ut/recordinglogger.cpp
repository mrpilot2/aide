#include "recordinglogger.hpp"

#include <algorithm>

using aide::LogLevel;
using aide::test::RecordingLogger;

const std::vector<RecordingLogger::Line>& RecordingLogger::lines() const
{
    return m_lines;
}

bool RecordingLogger::hasWarning() const
{
    return std::ranges::any_of(
        m_lines, [](const Line& line) { return line.level == LogLevel::Warn; });
}

void RecordingLogger::flush() {}

void RecordingLogger::setLevel([[maybe_unused]] LogLevel level) {}

void RecordingLogger::doLogTrace(std::string_view message) const
{
    record(LogLevel::Trace, message);
}

void RecordingLogger::doLogDebug(std::string_view message) const
{
    record(LogLevel::Debug, message);
}

void RecordingLogger::doLogInfo(std::string_view message) const
{
    record(LogLevel::Info, message);
}

void RecordingLogger::doLogWarn(std::string_view message) const
{
    record(LogLevel::Warn, message);
}

void RecordingLogger::doLogError(std::string_view message) const
{
    record(LogLevel::Error, message);
}

void RecordingLogger::doLogCritical(std::string_view message) const
{
    record(LogLevel::Critical, message);
}

void RecordingLogger::record(LogLevel level, std::string_view message) const
{
    m_lines.push_back({level, std::string(message)});
}
