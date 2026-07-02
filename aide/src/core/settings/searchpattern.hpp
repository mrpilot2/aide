#ifndef AIDE_SETTINGS_SEARCH_PATTERN_HPP
#define AIDE_SETTINGS_SEARCH_PATTERN_HPP

#include <algorithm>

#include <QChar>
#include <QString>
#include <QStringList>

namespace aide::core
{
    /**
     * @brief Splits a search pattern into independent words on whitespace.
     *
     * Consecutive whitespace is collapsed and leading/trailing whitespace is
     * ignored, so an empty or all-whitespace pattern yields an empty list.
     * QString::simplified() normalises every run of whitespace (spaces, tabs,
     * newlines) to a single space, so a plain split on the space character is
     * enough - no regular expression is required.
     */
    [[nodiscard]] inline QStringList tokenizeSearchPattern(
        const QString& pattern)
    {
        return pattern.simplified().split(QChar(' '), Qt::SkipEmptyParts);
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
