

#ifndef AIDE_SHOW_KEY_MAP_HPP
#define AIDE_SHOW_KEY_MAP_HPP

#include <actionregistryinterface.hpp>

#include "keymappagewidgetinterface.hpp"
#include "keymaptreemodel.hpp"

namespace aide::core
{
    class ShowKeyMap
    {
    public:
        ShowKeyMap(ActionRegistryInterfacePtr registry,
                   KeyMapPageWidgetRawPtr widget);

        void fillTreeView();

        [[nodiscard]] const std::shared_ptr<KeyMapTreeModel>& getTreeModel()
            const;

    private:
        const ActionRegistryInterfacePtr actionRegistry;

        KeyMapPageWidgetRawPtr keymapWidget;

        std::shared_ptr<KeyMapTreeModel> treeModel;
    };
} // namespace aide::core

#endif // AIDE_SHOW_KEY_MAP_HPP
