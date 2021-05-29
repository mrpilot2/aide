
#ifndef AIDE_KEYMAP_PAGE_WIDGET_INTERFACE_HPP
#define AIDE_KEYMAP_PAGE_WIDGET_INTERFACE_HPP

class QAbstractItemModel;

namespace aide::core
{
    class KeyMapPageWidgetInterface
    {
    public:
        virtual ~KeyMapPageWidgetInterface() = default;

        virtual void setTreeModel(
            std::shared_ptr<QAbstractItemModel> model) = 0;
    };

    using KeyMapPageWidgetRawPtr = KeyMapPageWidgetInterface*;

} // namespace aide::core

#endif // AIDE_KEYMAP_PAGE_WIDGET_INTERFACE_HPP
