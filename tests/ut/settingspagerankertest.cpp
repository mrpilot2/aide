#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "settings/settingspageranker.hpp"

using aide::core::SettingsPageRanker;

namespace
{
    constexpr auto kLow  = 0.2;
    constexpr auto kMid  = 0.5;
    constexpr auto kHigh = 0.8;
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
