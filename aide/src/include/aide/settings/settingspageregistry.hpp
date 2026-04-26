#ifndef AIDE_SETTINGS_PAGE_REGISTRY_HPP
#define AIDE_SETTINGS_PAGE_REGISTRY_HPP

#include <vector>

#include <aide/settings/settingspage.hpp>

namespace aide::core
{
    using SettingsPageList = std::vector<SettingsPagePtr>;

    class SettingsPageRegistry
    {
    public:
        SettingsPageRegistry()                                       = default;
        ~SettingsPageRegistry()                                      = default;
        SettingsPageRegistry(const SettingsPageRegistry&)            = delete;
        SettingsPageRegistry& operator=(const SettingsPageRegistry&) = delete;
        SettingsPageRegistry(SettingsPageRegistry&&)                 = delete;
        SettingsPageRegistry& operator=(SettingsPageRegistry&&)      = delete;

        void addPage(SettingsPagePtr page);

        [[nodiscard]] const SettingsPageList& settingsPages() const;

    private:
        SettingsPageList m_settingsPages;
    };
} // namespace aide::core

#endif // AIDE_SETTINGS_PAGE_REGISTRY_HPP
