#include "mocksettingspage.hpp"

#include <array>

#include <QApplication>
#include <QWidget>

using aide::test::MockSettingsPage;

MockSettingsPage::MockSettingsPage()
    : MockSettingsPage(HierarchicalId("Test")("Mock"))
{}

MockSettingsPage::MockSettingsPage(const aide::HierarchicalId& group)
    : SettingsPage(group)
{
    // NOLINTNEXTLINE
    std::array<char*, 1> appName{{const_cast<char*>("aide_test")}};
    int numberOfArgs{1};

    QApplication app(numberOfArgs, appName.data());

    mockWidget = std::make_shared<QWidget>();
}

QWidget* MockSettingsPage::widget()
{
    return mockWidget.get();
}

void MockSettingsPage::simulateModified(bool modified)
{
    pageIsModified = modified;
}

bool MockSettingsPage::isModified() const
{
    return pageIsModified;
}
