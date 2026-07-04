#include "settingspageranker.hpp"

using aide::core::SettingsPageRanker;

namespace
{
    constexpr double kNoMatchScore = 0.0;
} // namespace

std::optional<std::size_t> SettingsPageRanker::bestPage(
    const std::vector<double>& scores)
{
    if (scores.empty()) { return std::nullopt; }

    std::size_t best = 0;
    for (std::size_t i = 1; i < scores.size(); ++i) {
        if (scores[i] > scores[best]) { best = i; }
    }
    return best;
}

std::optional<std::size_t> SettingsPageRanker::bestPage(
    const std::vector<double>& scores, std::optional<std::size_t> currentPage)
{
    if (currentPage.has_value() && *currentPage < scores.size() &&
        scores[*currentPage] > kNoMatchScore) {
        return currentPage;
    }

    return bestPage(scores);
}
