#include "mocksettingspage.hpp"

#include <utility>

using aide::test::MockSettingsPage;

MockSettingsPage::MockSettingsPage()
    : SettingsPage(HierarchicalId("Test")("Mock"))
{}
