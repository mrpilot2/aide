#include <string>

#include <catch2/catch_test_macros.hpp>

#include <QString>

#include <aide/helptopicid.hpp>

#include "capturedmacrolog.hpp"

using aide::HelpTopicId;
using aide::test::CapturedMacroLog;

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
