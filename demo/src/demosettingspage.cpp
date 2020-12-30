#include "demosettingspage.hpp"

#include <utility>

#include <aide/hierarchicalid.hpp>

using aide::HierarchicalId;
using aide::core::SettingsPage;

DemoSettingsPage::DemoSettingsPage(aide::HierarchicalId group)
    : SettingsPage(std::move(group))
{}

DemoSettingsPage::~DemoSettingsPage() = default;
