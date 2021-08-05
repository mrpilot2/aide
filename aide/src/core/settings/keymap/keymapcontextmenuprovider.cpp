
#include "keymapcontextmenuprovider.hpp"

using aide::core::KeymapContextMenuProvider;
using aide::core::KeyMapPageWidgetRawPtr;

KeymapContextMenuProvider::KeymapContextMenuProvider(
    KeyMapPageWidgetRawPtr view)
    : keymapView{view}
{}

void KeymapContextMenuProvider::createAndShowContextMenu()
{
    keymapView->showContextMenu({});
}
