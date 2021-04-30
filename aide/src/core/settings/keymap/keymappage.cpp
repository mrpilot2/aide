
#include "keymappage.hpp"

#include <QLabel>

using aide::core::KeymapPage;

KeymapPage::KeymapPage()
    : SettingsPage(HierarchicalId("Keymap"))
    , m_widget(new aide::gui::KeymapPageWidget())
{}

QWidget* KeymapPage::widget()
{
    return m_widget;
}

bool KeymapPage::isModified() const
{
    return false;
}

void KeymapPage::reset() {}
void KeymapPage::apply() {}
