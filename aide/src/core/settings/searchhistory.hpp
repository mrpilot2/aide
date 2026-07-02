#ifndef AIDE_SETTINGS_SEARCH_HISTORY_HPP
#define AIDE_SETTINGS_SEARCH_HISTORY_HPP

#include <QString>
#include <QStringList>

namespace aide
{
    class SettingsInterface;
} // namespace aide

namespace aide::core
{
    /**
     * @brief Persisted list of the most recent settings-dialog searches.
     *
     * Owns the last @ref MAX_ENTRIES committed search strings, most recent
     * first. Committing deduplicates (a repeat moves to the front instead of
     * duplicating) and caps the list, dropping the oldest entry when full. The
     * list is stored through the injected @ref SettingsInterface as
     * unversionable UI state.
     */
    class SearchHistory
    {
    public:
        static constexpr int MAX_ENTRIES = 5;

        explicit SearchHistory(SettingsInterface& unversionableSettings);

        void commit(const QString& pattern);

        [[nodiscard]] QStringList entries() const;

    private:
        SettingsInterface& settings;
    };
} // namespace aide::core

#endif // AIDE_SETTINGS_SEARCH_HISTORY_HPP
