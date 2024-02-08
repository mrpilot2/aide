
#include "keymappage.hpp"

#include <actionregistryinterface.hpp>
#include <utility>

#include <QAction>
#include <QLabel>

#include "showkeymap.hpp"
#include "treeitem.hpp"

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
    const auto& actions{actionRegistry->actions()};

    return std::any_of(
        actions.begin(), actions.end(), [this](const auto& keyActionPair) {
            const auto& [id, action] = keyActionPair;
            if (auto item =
                    showUseCase.getTreeModel()->findItemForActionId(id)) {
                return !action.areKeySequencesTheSame(
                    QKeySequence::listFromString(
                        item.value()->data(1).toString()),
                    action.getActiveKeySequences());
            }
            return false;
        });
}

void KeymapPage::reset()
{
    showUseCase.fillTreeView();
}

void KeymapPage::apply()
{
    for (const auto& [id, action] : actionRegistry->actions()) {
        if (auto item = showUseCase.getTreeModel()->findItemForActionId(id)) {
            auto treeModelShortcuts =
                QKeySequence::listFromString(item.value()->data(1).toString());
            if (!Action::areKeySequencesTheSame(
                    treeModelShortcuts, action.getActiveKeySequences())) {
                actionRegistry->modifyShortcutsForAction(id,
                                                         treeModelShortcuts);
            }
        }
    }
}

aide::core::KeyMapPageWidgetInterface* KeymapPage::keyMapWidget() const
{
    return m_widget;
}

const std::shared_ptr<KeyMapTreeModel>& KeymapPage::getTreeModel() const
{
    return showUseCase.getTreeModel();
}
