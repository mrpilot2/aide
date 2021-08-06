
#include "keymappagewidgetcontroller.hpp"

#include "settings/keymap/keymapcontextmenuprovider.hpp"
#include "settings/keymap/keymaptreemodel.hpp"

using aide::core::KeymapContextMenuProvider;
using aide::core::KeyMapPageWidgetInterface;
using aide::core::KeyMapTreeModel;
using aide::gui::KeyMapPageWidgetController;

aide::gui::KeyMapPageWidgetController::KeyMapPageWidgetController(
    const std::shared_ptr<KeyMapTreeModel>& treeModel,
    core::KeyMapPageWidgetInterface* view)
    : keyMapTreeModel(treeModel)
    , keyMapView(view)
{}

void aide::gui::KeyMapPageWidgetController::requestContextMenuForIndex(
    const QModelIndex& index)
{
    KeymapContextMenuProvider creator{keyMapTreeModel.get(), keyMapView};
    creator.createAndShowContextMenu(index);
}

void KeyMapPageWidgetController::onUserRequestedContextMenuViaDoubleClick(
    const QModelIndex& index)
{
    requestContextMenuForIndex(index);
}
