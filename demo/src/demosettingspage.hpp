#ifndef AIDE_DEMO_SETTINGS_PAGE_HPP
#define AIDE_DEMO_SETTINGS_PAGE_HPP

#include <aide/hierarchicalid.hpp>
#include <aide/settings/settingspage.hpp>

class DemoSettingsPage : public aide::core::SettingsPage
{
public:
    DemoSettingsPage(aide::HierarchicalId group);

    ~DemoSettingsPage() override;
};

#endif // AIDE_DEMO_SETTINGS_PAGE_HPP
