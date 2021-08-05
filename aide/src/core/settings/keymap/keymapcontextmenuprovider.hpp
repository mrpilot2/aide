
#ifndef AIDE_KEYMAP_CONTEXT_MENU_PROVIDER_HPP
#define AIDE_KEYMAP_CONTEXT_MENU_PROVIDER_HPP

#include "keymappagewidgetinterface.hpp"

namespace aide::core
{
    class KeymapContextMenuProvider
    {
    public:
        KeymapContextMenuProvider(KeyMapPageWidgetRawPtr view);

        void createAndShowContextMenu();

    private:
        KeyMapPageWidgetRawPtr keymapView;
    };
} // namespace aide::core

#endif // AIDE_KEYMAP_CONTEXT_MENU_PROVIDER_HPP
