#include <optional>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "settings/settingspageranker.hpp"

using aide::core::SettingsPageRanker;

namespace
{
    constexpr auto kNoMatch = 0.0;
    constexpr auto kLow     = 0.2;
    constexpr auto kMid     = 0.5;
    constexpr auto kHigh    = 0.8;
} // namespace

TEST_CASE("Ranking settings pages by score")
{
    SECTION("selects the highest-scoring page")
    {
        const std::vector<double> scores{kLow, kHigh, kMid};

        REQUIRE(SettingsPageRanker::bestPage(scores) == 1);
    }

    SECTION("breaks ties by tree order, keeping the earliest page")
    {
        const std::vector<double> scores{kLow, kMid, kMid};

        REQUIRE(SettingsPageRanker::bestPage(scores) == 1);
    }

    SECTION("returns the first page when every score is equal")
    {
        const std::vector<double> scores{kMid, kMid, kMid};

        REQUIRE(SettingsPageRanker::bestPage(scores) == 0);
    }

    SECTION("returns no page when the list is empty")
    {
        REQUIRE_FALSE(
            SettingsPageRanker::bestPage(std::vector<double>{}).has_value());
    }
}

TEST_CASE("Sticky pinning of the current settings page")
{
    SECTION("pins the current page even when another scores higher")
    {
        const std::vector<double> scores{kHigh, kLow, kMid};

        REQUIRE(SettingsPageRanker::bestPage(scores, std::size_t{1}) == 1);
    }

    SECTION("releases the pin when the current page stops matching")
    {
        const std::vector<double> scores{kHigh, kNoMatch, kMid};

        REQUIRE(SettingsPageRanker::bestPage(scores, std::size_t{1}) == 0);
    }

    SECTION("transfers the pin to whichever page the caller marks current")
    {
        const std::vector<double> scores{kMid, kLow, kLow};

        REQUIRE(SettingsPageRanker::bestPage(scores, std::size_t{2}) == 2);
    }

    SECTION("ignores a current index that is out of range")
    {
        const std::vector<double> scores{kLow, kHigh};

        REQUIRE(SettingsPageRanker::bestPage(scores, std::size_t{9}) == 1);
    }

    SECTION("falls back to best score when there is no current page")
    {
        const std::vector<double> scores{kLow, kHigh, kMid};

        REQUIRE(SettingsPageRanker::bestPage(scores, std::nullopt) == 1);
    }
}
