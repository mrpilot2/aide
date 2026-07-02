#ifndef AIDE_SETTINGS_TEXT_MATCHER_HPP
#define AIDE_SETTINGS_TEXT_MATCHER_HPP

#include <QString>
#include <QStringList>

namespace aide::core
{
    /**
     * @brief Pure exactness scoring of text against search words.
     *
     * Scores how closely a piece of text matches a search word: the score is
     * the fraction of the text that the matched substring covers, so a shorter
     * text bearing the word scores higher than a longer one where the word is
     * buried (e.g. "font" scores 0.8 against "Font:" but 0.25 against "Change
     * font size"). Matching is case-insensitive exact-substring matching,
     * consistent with the rest of the settings search. The module operates on
     * plain strings only - it never traverses Qt widgets - so it can be unit
     * tested without instantiating any GUI.
     */
    class TextMatcher
    {
    public:
        /**
         * @brief Score of a single @p word against @p text.
         *
         * Returns the matched-substring length divided by the full text
         * length when @p word is a case-insensitive substring of @p text, and
         * 0 when it is not contained or @p text is empty.
         */
        [[nodiscard]] static double score(const QString& text,
                                          const QString& word);

        /**
         * @brief Total score of @p texts against @p words.
         *
         * For every word the best (highest) score across all @p texts is
         * taken, and those best per-word scores are summed. Words that match
         * none of the texts contribute 0.
         */
        [[nodiscard]] static double score(const QStringList& texts,
                                          const QStringList& words);
    };
} // namespace aide::core

#endif // AIDE_SETTINGS_TEXT_MATCHER_HPP
