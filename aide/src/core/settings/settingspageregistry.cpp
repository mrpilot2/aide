#include "aide/settings/settingspageregistry.hpp"

#include <algorithm>
#include <stdexcept>
#include <vector>

#include "settings/settingspage.hpp"

using aide::core::SettingsPageList;
using aide::core::SettingsPageRegistry;
using aide::core::SettingsPageUniquePtr;

class SettingsPageRegistryPrivate
{
public:
    ~SettingsPageRegistryPrivate()                                  = default;
    SettingsPageRegistryPrivate(const SettingsPageRegistryPrivate&) = delete;
    SettingsPageRegistryPrivate& operator=(const SettingsPageRegistryPrivate&) =
        delete;
    SettingsPageRegistryPrivate(SettingsPageRegistryPrivate&&) = delete;
    SettingsPageRegistryPrivate& operator=(SettingsPageRegistryPrivate&&) =
        delete;

    static SettingsPageRegistryPrivate& instance()
    {
        static SettingsPageRegistryPrivate privateRegistry;
        return privateRegistry;
    }

    void addPage(SettingsPageUniquePtr page)
    {
        auto group = page->group();
        if (std::find_if(m_settingsPages.begin(), m_settingsPages.end(),
                         [group](const auto& elem) {
                             return group == elem->group();
                         }) != m_settingsPages.end()) {
            throw std::invalid_argument(
                "SettingsPageRegistry: duplicate registration for group " +
                page->group().name() + " detected.");
        }
        m_settingsPages.push_back(std::move(page));
    }

    [[nodiscard]] const SettingsPageList& settingsPages() const
    {
        return m_settingsPages;
    }

    void deleteAllPages() noexcept { m_settingsPages.clear(); }

private:
    SettingsPageRegistryPrivate() = default;

    SettingsPageList m_settingsPages;
};

void SettingsPageRegistry::addPage(SettingsPageUniquePtr page)
{
    SettingsPageRegistryPrivate::instance().addPage(std::move(page));
}

const SettingsPageList& SettingsPageRegistry::settingsPages()
{
    return SettingsPageRegistryPrivate::instance().settingsPages();
}

void SettingsPageRegistry::deleteAllPages() noexcept
{
    SettingsPageRegistryPrivate::instance().deleteAllPages();
}
