#include "aide/settings/settingspageregistry.hpp"

#include <algorithm>
#include <stdexcept>

using aide::core::SettingsPageList;
using aide::core::SettingsPagePtr;
using aide::core::SettingsPageRegistry;

void SettingsPageRegistry::addPage(SettingsPagePtr page)
{
    auto group = page->group();
    if (std::ranges::find_if(m_settingsPages, [group](const auto& elem) {
            return group == elem->group();
        }) != m_settingsPages.end()) {
        throw std::invalid_argument(
            "SettingsPageRegistry: duplicate registration for group " +
            page->group().name() + " detected.");
    }
    m_settingsPages.push_back(std::move(page));
}

const SettingsPageList& SettingsPageRegistry::settingsPages() const
{
    return m_settingsPages;
}
