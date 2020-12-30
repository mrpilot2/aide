#include "mocksettingspage.hpp"

using aide::test::MockSettingsPage;

MockSettingsPage::MockSettingsPage()
    : SettingsPage(HierarchicalId("Test")("Mock"))
{}

MockSettingsPage::MockSettingsPage(const aide::HierarchicalId& group)
    : SettingsPage(group)
{}
