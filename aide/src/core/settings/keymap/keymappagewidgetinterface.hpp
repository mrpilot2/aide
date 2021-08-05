
#ifndef AIDE_KEYMAP_PAGE_WIDGET_INTERFACE_HPP
#define AIDE_KEYMAP_PAGE_WIDGET_INTERFACE_HPP

#include <memory>
#include <string>
#include <vector>

#include <QKeySequence>

class QAbstractItemModel;

namespace aide::core
{
    enum class ContextMenuItemType
    {
        ADD_KEYBOARD_SHORTCUT = 0,
        REMOVE_SHORTCUT       = 1,
        RESET_TO_DEFAULTS     = 2,
        SEPARATOR             = 3
    };

    struct ContextMenuEntry
    {
        ContextMenuItemType type{ContextMenuItemType::ADD_KEYBOARD_SHORTCUT};
        std::string displayText;
        QKeySequence sequence;
    };

    using ContextMenuEntries = std::vector<ContextMenuEntry>;

    class KeyMapPageWidgetInterface
    {
    public:
        virtual ~KeyMapPageWidgetInterface() = default;

        virtual void setTreeModel(
            std::shared_ptr<QAbstractItemModel> model) = 0;

        virtual void showContextMenu(const ContextMenuEntries& entries) = 0;
    };

    using KeyMapPageWidgetRawPtr = KeyMapPageWidgetInterface*;

} // namespace aide::core

#endif // AIDE_KEYMAP_PAGE_WIDGET_INTERFACE_HPP
