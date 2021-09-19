
#ifndef AIDE_KEYMAP_CONTEXT_MENU_PROVIDER_HPP
#define AIDE_KEYMAP_CONTEXT_MENU_PROVIDER_HPP

#include <QModelIndex>
#include <QObject>

#include "keymappagewidgetinterface.hpp"

namespace aide::core
{
    class KeyMapTreeModel;

    class KeymapContextMenuProvider : public QObject
    {
        Q_OBJECT
    public:
        KeymapContextMenuProvider(KeyMapTreeModel* model,
                                  KeyMapPageWidgetRawPtr view);

        void createAndShowContextMenu(const QModelIndex& index);

    private:
        KeyMapTreeModel* treeModel;

        KeyMapPageWidgetRawPtr keymapView;
    };
} // namespace aide::core

#endif // AIDE_KEYMAP_CONTEXT_MENU_PROVIDER_HPP
