#include "settingspageranker.hpp"

using aide::core::SettingsPageRanker;

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
