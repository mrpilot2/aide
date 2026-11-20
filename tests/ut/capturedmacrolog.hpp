#ifndef AIDE_CAPTURED_MACRO_LOG_HPP
#define AIDE_CAPTURED_MACRO_LOG_HPP

#include <memory>
#include <sstream>
#include <string>

namespace spdlog
{
    class logger;
} // namespace spdlog

namespace aide::test
{
    // For code that logs through the global AIDE_LOG_* macros instead of an
    // injected LoggerPtr (e.g. value types such as HelpTopicId): while alive,
    // replaces the macros' logger with one writing into a string stream, and
    // restores the original on destruction. Goes through LoggerMacros, not
    // spdlog directly: in shared builds on Windows and macOS the test binary
    // and AideLogger each have their own spdlog registry.
    class CapturedMacroLog
    {
    public:
        CapturedMacroLog();
        ~CapturedMacroLog();

        CapturedMacroLog(const CapturedMacroLog&)            = delete;
        CapturedMacroLog& operator=(const CapturedMacroLog&) = delete;
        CapturedMacroLog(CapturedMacroLog&&)                 = delete;
        CapturedMacroLog& operator=(CapturedMacroLog&&)      = delete;

        [[nodiscard]] std::string text() const;

    private:
        std::shared_ptr<spdlog::logger> m_previous;
        std::ostringstream m_out;
    };
} // namespace aide::test

#endif // AIDE_CAPTURED_MACRO_LOG_HPP
