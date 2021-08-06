
#ifndef AIDE_KEYMAP_PAGE_WIDGET_CONTROLLER_HPP
#define AIDE_KEYMAP_PAGE_WIDGET_CONTROLLER_HPP

#include <memory>

#include <QObject>

namespace aide::core
{
    class KeyMapTreeModel;
    class KeyMapPageWidgetInterface;
} // namespace aide::core

namespace aide::gui
{
    class KeyMapPageWidgetController : public QObject
    {
        Q_OBJECT
    public:
        KeyMapPageWidgetController(
            const std::shared_ptr<core::KeyMapTreeModel>& treeModel,
            core::KeyMapPageWidgetInterface* view);

        void requestContextMenuForIndex(const QModelIndex& index);

    public slots:

        void onUserRequestedContextMenuViaDoubleClick(const QModelIndex& index);

    private:
        const std::shared_ptr<core::KeyMapTreeModel>& keyMapTreeModel;
        core::KeyMapPageWidgetInterface* keyMapView;
    };

    using KeyMapPageControllerPtr = std::shared_ptr<KeyMapPageWidgetController>;
} // namespace aide::gui

#endif // AIDE_KEYMAP_PAGE_WIDGET_CONTROLLER_HPP
