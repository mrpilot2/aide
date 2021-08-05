
#include "keymappage.hpp"

#include <actionregistryinterface.hpp>
#include <utility>

#include <QLabel>

#include "showkeymap.hpp"

using aide::core::KeymapPage;
using aide::core::KeyMapTreeModel;

KeymapPage::KeymapPage(ActionRegistryInterfacePtr registry,
                       KeyMapPageWidgetInterface* widget)
    : SettingsPage(HierarchicalId("Keymap"))
    , actionRegistry(std::move(registry))
    , m_widget(widget)
    , showUseCase(actionRegistry, m_widget)
{}

QWidget* KeymapPage::widget()
{
    return dynamic_cast<QWidget*>(m_widget);
}

bool KeymapPage::isModified() const
{
    return false;
}

void KeymapPage::reset()
{
    showUseCase.fillTreeView();
}

void KeymapPage::apply() {}

aide::core::KeyMapPageWidgetInterface* KeymapPage::keyMapWidget() const
{
    return m_widget;
}

const std::shared_ptr<KeyMapTreeModel>& aide::core::KeymapPage::getTreeModel()
    const
{
    return showUseCase.getTreeModel();
}
