#include "mocksettingspage.hpp"

#include <memory>

#include <QLabel>
#include <QString>
#include <QWidget>

using aide::test::MockSettingsPage;

MockSettingsPage::MockSettingsPage()
    : MockSettingsPage(HierarchicalId("Test")("Mock"))
{}

MockSettingsPage::MockSettingsPage(const HierarchicalId& group)
    : MockSettingsPage(group, QString())
{}

MockSettingsPage::MockSettingsPage(const HierarchicalId& group,
                                   const QString& searchableText)
    : SettingsPage(group)
    , mockWidget{std::make_shared<QWidget>()}
{
    if (!searchableText.isEmpty()) {
        new QLabel(searchableText, mockWidget.get());
    }
}

MockSettingsPage::~MockSettingsPage() = default;

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
