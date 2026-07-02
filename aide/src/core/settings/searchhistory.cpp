#include "searchhistory.hpp"

#include "commonsettingskeys.hpp"
#include "settingsinterface.hpp"

using aide::core::SearchHistory;
using aide::core::settings::KEYS;

SearchHistory::SearchHistory(SettingsInterface& unversionableSettings)
    : settings{unversionableSettings}
{}

void SearchHistory::commit(const QString& pattern)
{
    if (pattern.trimmed().isEmpty()) { return; }

    QStringList history = entries();
    history.removeAll(pattern);
    history.prepend(pattern);
    while (history.size() > MAX_ENTRIES) {
        history.removeLast();
    }
    settings.setValue(KEYS().UI.SETTINGS_DIALOG_SEARCH_HISTORY_KEY, history);
}

QStringList SearchHistory::entries() const
{
    return settings.value(KEYS().UI.SETTINGS_DIALOG_SEARCH_HISTORY_KEY)
        .toStringList();
}
