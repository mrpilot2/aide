#include "aide/settings/settingspage.hpp"

using aide::HierarchicalId;
using aide::core::SettingsPage;

SettingsPage::SettingsPage(HierarchicalId group)
    : settingsGroup{std::move(group)}
{}

SettingsPage::~SettingsPage() = default;

const HierarchicalId& SettingsPage::group() const
{
    return settingsGroup;
}
