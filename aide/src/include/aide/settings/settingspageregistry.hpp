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
        SettingsPageRegistry()                            = delete;
        ~SettingsPageRegistry()                           = delete;
        SettingsPageRegistry(const SettingsPageRegistry&) = delete;
        SettingsPageRegistry& operator=(const SettingsPageRegistry&) = delete;
        SettingsPageRegistry(SettingsPageRegistry&&)                 = delete;
        SettingsPageRegistry& operator=(SettingsPageRegistry&&) = delete;

        static void addPage(SettingsPagePtr page);

        static const SettingsPageList& settingsPages();

        static void deleteAllPages() noexcept;
    };
} // namespace aide::core

#endif // AIDE_SETTINGS_PAGE_REGISTRY_HPP
