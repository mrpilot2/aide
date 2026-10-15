#include <memory>
#include <sstream>
#include <string>

#include <catch2/catch_test_macros.hpp>

#include <QString>

#include <aide/helptopicid.hpp>
#include <aide/logger/log_helper_macros.hpp>
#include <spdlog/sinks/ostream_sink.h>
#include <spdlog/spdlog.h>

using aide::HelpTopicId;

namespace
{
    // Temporarily replaces the logger behind the AIDE_LOG_* macros with one
    // that writes into a string stream, restoring the original afterwards.
    class CapturedMacroLog
    {
    public:
        CapturedMacroLog()
            : m_previous(spdlog::get(AIDE_DEFAULT_MACRO_LOGGER))
        {
            spdlog::drop(AIDE_DEFAULT_MACRO_LOGGER);
            auto sink = std::make_shared<spdlog::sinks::ostream_sink_mt>(m_out);
            auto logger = std::make_shared<spdlog::logger>(
                AIDE_DEFAULT_MACRO_LOGGER, sink);
            logger->set_level(spdlog::level::trace);
            spdlog::register_logger(logger);
        }

        ~CapturedMacroLog()
        {
            spdlog::drop(AIDE_DEFAULT_MACRO_LOGGER);
            if (m_previous != nullptr) { spdlog::register_logger(m_previous); }
        }

        CapturedMacroLog(const CapturedMacroLog&)            = delete;
        CapturedMacroLog& operator=(const CapturedMacroLog&) = delete;
        CapturedMacroLog(CapturedMacroLog&&)                 = delete;
        CapturedMacroLog& operator=(CapturedMacroLog&&)      = delete;

        [[nodiscard]] std::string text() const { return m_out.str(); }

    private:
        std::shared_ptr<spdlog::logger> m_previous;
        std::ostringstream m_out;
    };
} // namespace

TEST_CASE("A default-constructed help topic id is empty", "[HelpTopicId]")
{
    const HelpTopicId id;

    REQUIRE(id.isEmpty());
    REQUIRE(id.value().isEmpty());
}

TEST_CASE("A help topic id accepts relative paths", "[HelpTopicId]")
{
    const CapturedMacroLog log;

    SECTION("a single segment")
    {
        const HelpTopicId id{"keymap"};

        REQUIRE_FALSE(id.isEmpty());
        REQUIRE(id.value() == QString("keymap"));
    }

    SECTION("nested segments")
    {
        const HelpTopicId id{"settings/keymap"};

        REQUIRE(id.value() == QString("settings/keymap"));
    }

    REQUIRE(log.text().empty());
}

TEST_CASE("A help topic id rejects invalid input as empty plus a warning",
          "[HelpTopicId]")
{
    const CapturedMacroLog log;

    SECTION("a leading slash")
    {
        REQUIRE(HelpTopicId{"/keymap"}.isEmpty());
        REQUIRE(log.text().find("/keymap") != std::string::npos);
    }

    SECTION("a parent-directory segment")
    {
        REQUIRE(HelpTopicId{"a/../b"}.isEmpty());
        REQUIRE(log.text().find("a/../b") != std::string::npos);
    }

    SECTION("a query character")
    {
        REQUIRE(HelpTopicId{"a?b"}.isEmpty());
        REQUIRE(log.text().find("a?b") != std::string::npos);
    }

    SECTION("a fragment character")
    {
        REQUIRE(HelpTopicId{"a#b"}.isEmpty());
        REQUIRE(log.text().find("a#b") != std::string::npos);
    }
}

TEST_CASE("An empty help topic id input is not a warning", "[HelpTopicId]")
{
    const CapturedMacroLog log;

    REQUIRE(HelpTopicId{""}.isEmpty());
    REQUIRE(log.text().empty());
}

TEST_CASE("Help topic ids compare by value", "[HelpTopicId]")
{
    REQUIRE(HelpTopicId{"keymap"} == HelpTopicId{"keymap"});
    REQUIRE(HelpTopicId{"keymap"} != HelpTopicId{"appearance"});
    REQUIRE(HelpTopicId{"appearance"} < HelpTopicId{"keymap"});
}
