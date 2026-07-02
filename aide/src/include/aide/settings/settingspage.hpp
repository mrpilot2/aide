#ifndef AIDE_SETTINGS_PAGE_HPP
#define AIDE_SETTINGS_PAGE_HPP

#include <memory>

#include <QStringList>

#include <aide/hierarchicalid.hpp>

class QString;
class QWidget;

namespace aide::core
{
    class SettingsPage
    {
    public:
        explicit SettingsPage(HierarchicalId group);

        virtual ~SettingsPage();

        SettingsPage(const SettingsPage&)            = delete;
        SettingsPage& operator=(const SettingsPage&) = delete;
        SettingsPage(SettingsPage&&)                 = delete;
        SettingsPage& operator=(SettingsPage&&)      = delete;

        [[nodiscard]] const HierarchicalId& group() const;

        [[nodiscard]] virtual QWidget* widget() = 0;

        [[nodiscard]] virtual bool isModified() const = 0;

        /**
         * @brief Returns whether the page's content matches a search pattern.
         *
         * The default implementation performs a case-insensitive substring
         * search across the text of common child widgets (labels, buttons,
         * check boxes, group boxes) on the page. Pages can override this to
         * search additional or custom content.
         */
        [[nodiscard]] virtual bool matches(const QString& pattern);

        /**
         * @brief Scores how well the page's content matches the search words.
         *
         * For every word the best (closest) match among the page's matchable
         * child widgets (labels, buttons, check boxes, group boxes) is taken -
         * where a match's score is the fraction of the widget's text it covers
         * - and those best per-word scores are summed. A higher score means a
         * more exact match, so the settings dialog can auto-select the page
         * that best fits the search. Words matching no widget contribute 0.
         */
        [[nodiscard]] virtual double score(const QStringList& words);

        /**
         * @brief Highlights child widgets whose text matches a search pattern.
         *
         * The default implementation applies a brownish-orange border via a
         * style sheet to every common child widget (labels, buttons, check
         * boxes, group boxes) whose text contains the pattern, and removes the
         * border from widgets that do not match. While a search is active every
         * other widget on the page - regardless of its type, so client-provided
         * pages are handled too - is dimmed via a reduced-opacity effect so the
         * matching controls stand out, while matching widgets and their
         * containing ancestors stay at full opacity. Matching is
         * case-insensitive substring matching, consistent with matches(). An
         * empty pattern removes all highlights and dimming.
         */
        virtual void highlight(const QString& pattern);

        virtual void reset() = 0;

        virtual void apply() = 0;

    private:
        HierarchicalId settingsGroup;
    };

    using SettingsPagePtr = std::shared_ptr<SettingsPage>;
} // namespace aide::core

#endif // AIDE_SETTINGS_PAGE_HPP
