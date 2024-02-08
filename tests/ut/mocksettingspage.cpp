#include "mocksettingspage.hpp"

#include <array>

#include <QApplication>
#include <QWidget>

using aide::test::MockSettingsPage;

MockSettingsPage::MockSettingsPage()
    : MockSettingsPage(HierarchicalId("Test")("Mock"))
{}

MockSettingsPage::MockSettingsPage(const HierarchicalId& group)
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

void MockSettingsPage::simulateModified(const bool modified)
{
    pageIsModified = modified;
}

bool MockSettingsPage::isModified() const
{
    return pageIsModified;
}

void MockSettingsPage::reset()
{
    ++resetCounter;
    resetWasCalled = true;
}

bool MockSettingsPage::wasResetCalled() const
{
    return resetWasCalled;
}

void MockSettingsPage::clearResetWasCalled()
{
    resetCounter   = 0;
    resetWasCalled = false;
}

uint16_t MockSettingsPage::numberOfTimesResetWasCalled() const
{
    return resetCounter;
}

void MockSettingsPage::apply()
{
    applyWasCalled = true;
}
bool MockSettingsPage::wasApplyCalled() const
{
    return applyWasCalled;
}
