
#include "showkeymap.hpp"

#include <utility>

using aide::core::ShowKeyMap;

ShowKeyMap::ShowKeyMap(aide::ActionRegistryInterfacePtr registry,
                       KeyMapPageWidgetRawPtr widget)
    : actionRegistry{std::move(registry)}
    , keymapWidget{widget}
{}

void ShowKeyMap::fillTreeView()
{
    if (keymapWidget != nullptr) {
        treeModel = std::make_shared<KeyMapTreeModel>(actionRegistry);
        keymapWidget->setTreeModel(treeModel);
    }
}
