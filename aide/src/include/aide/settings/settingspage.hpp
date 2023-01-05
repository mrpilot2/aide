#ifndef AIDE_SETTINGS_PAGE_HPP
#define AIDE_SETTINGS_PAGE_HPP

#include <memory>

#include <aide/hierarchicalid.hpp>

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

        virtual void reset() = 0;

        virtual void apply() = 0;

    private:
        HierarchicalId settingsGroup;
    };

    using SettingsPagePtr = std::shared_ptr<SettingsPage>;
} // namespace aide::core

#endif // AIDE_SETTINGS_PAGE_HPP
