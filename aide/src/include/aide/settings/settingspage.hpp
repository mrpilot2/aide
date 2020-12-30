#ifndef AIDE_SETTINGS_PAGE_HPP
#define AIDE_SETTINGS_PAGE_HPP

#include <memory>

#include <aide/hierarchicalid.hpp>

namespace aide::core
{
    class SettingsPage
    {
    public:
        explicit SettingsPage(HierarchicalId group);

        virtual ~SettingsPage();

        SettingsPage(const SettingsPage&) = delete;
        SettingsPage& operator=(const SettingsPage&) = delete;
        SettingsPage(SettingsPage&&)                 = delete;
        SettingsPage& operator=(SettingsPage&&) = delete;

        [[nodiscard]] const HierarchicalId& group() const;

    private:
        HierarchicalId settingsGroup;
    };

    using SettingsPageUniquePtr = std::unique_ptr<SettingsPage>;
} // namespace aide::core

#endif // AIDE_SETTINGS_PAGE_HPP
