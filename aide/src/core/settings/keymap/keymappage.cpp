
#include "keymappage.hpp"

#include <utility>

#include <QWidget>

using aide::core::KeymapPage;
using aide::core::KeyMapTreeModel;

KeymapPage::KeymapPage(const ActionRegistryInterfacePtr& registry,
                       KeyMapPageWidgetInterface* widget)
    : SettingsPage(HierarchicalId("Keymap"))
    , m_widget(widget)
    , m_controller(registry)
{}

QWidget* KeymapPage::widget()
{
    return dynamic_cast<QWidget*>(m_widget);
}

bool KeymapPage::isModified() const
{
    return m_controller.isModified();
}

void KeymapPage::reset()
{
    m_controller.reset();
    m_widget->setTreeModel(m_controller.treeModel());
}

void KeymapPage::apply()
{
    m_controller.apply();
}

aide::core::KeyMapPageWidgetInterface* KeymapPage::keyMapWidget() const
{
    return m_widget;
}

const std::shared_ptr<KeyMapTreeModel>& KeymapPage::getTreeModel() const
{
    return m_controller.treeModel();
}
