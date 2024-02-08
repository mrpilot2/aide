
#include "showkeymap.hpp"

#include <utility>

using aide::core::KeyMapTreeModel;
using aide::core::ShowKeyMap;

ShowKeyMap::ShowKeyMap(ActionRegistryInterfacePtr registry,
                       KeyMapPageWidgetRawPtr widget)
    : actionRegistry{std::move(registry)}
    , keymapWidget{widget}
    , treeModel(std::make_shared<KeyMapTreeModel>(actionRegistry))
{}

void ShowKeyMap::fillTreeView() const
{
    if (keymapWidget != nullptr) {
        treeModel->setupModelData();
        keymapWidget->setTreeModel(treeModel);
    }
}

const std::shared_ptr<KeyMapTreeModel>& ShowKeyMap::getTreeModel() const
{
    return treeModel;
}
