#ifndef AIDE_SETTINGS_SEARCH_PATTERN_HPP
#define AIDE_SETTINGS_SEARCH_PATTERN_HPP

#include <algorithm>

#include <QRegularExpression>
#include <QString>
#include <QStringList>

namespace aide::core
{
    /**
     * @brief Splits a search pattern into independent words on whitespace.
     *
     * Consecutive whitespace is collapsed and leading/trailing whitespace is
     * ignored, so an empty or all-whitespace pattern yields an empty list.
     */
    [[nodiscard]] inline QStringList tokenizeSearchPattern(
        const QString& pattern)
    {
        static const QRegularExpression whitespace{"\\s+"};
        return pattern.split(whitespace, Qt::SkipEmptyParts);
    }

    /**
     * @brief Whether the text contains any of the words (case-insensitive).
     *
     * Implements the OR semantics of the settings search: a text matches when
     * at least one search word is a case-insensitive substring of it. An empty
     * word list never matches.
     */
    [[nodiscard]] inline bool matchesAnyWord(const QString& text,
                                             const QStringList& words)
    {
        return std::ranges::any_of(words, [&text](const QString& word) {
            return text.contains(word, Qt::CaseInsensitive);
        });
    }
} // namespace aide::core

#endif // AIDE_SETTINGS_SEARCH_PATTERN_HPP
