#ifndef AIDE_SETTINGS_PAGE_RANKER_HPP
#define AIDE_SETTINGS_PAGE_RANKER_HPP

#include <cstddef>
#include <optional>
#include <vector>

namespace aide::core
{
    /**
     * @brief Decides which settings page best matches the current search.
     *
     * Given the per-page search scores of the currently visible pages in tree
     * order (as computed by @ref TextMatcher), the ranker selects the page the
     * dialog should auto-select: the highest-scoring one, with ties broken by
     * tree order (the earliest page wins). It is a pure decision function - it
     * neither traverses widgets nor touches persistence or GUI state - so it
     * can be unit tested against plain score lists.
     */
    class SettingsPageRanker
    {
    public:
        /**
         * @brief Index of the best page among @p scores, in tree order.
         *
         * Returns the index of the highest-scoring page; ties resolve to the
         * lowest index. Returns std::nullopt when @p scores is empty.
         */
        [[nodiscard]] static std::optional<std::size_t> bestPage(
            const std::vector<double>& scores);

        /**
         * @brief Index of the best page, keeping the current page sticky.
         *
         * Like @ref bestPage, but if @p currentPage refers to a page that still
         * matches (its score is greater than zero) it is pinned to the top
         * regardless of whether another page scores higher, so the view does
         * not jump away from a page the user is still looking at. The pin
         * releases as soon as the current page stops matching. An empty or
         * out-of-range @p currentPage disables pinning and falls back to plain
         * best-score selection.
         */
        [[nodiscard]] static std::optional<std::size_t> bestPage(
            const std::vector<double>& scores,
            std::optional<std::size_t> currentPage);
    };
} // namespace aide::core

#endif // AIDE_SETTINGS_PAGE_RANKER_HPP
