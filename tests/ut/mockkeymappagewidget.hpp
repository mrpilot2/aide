
#ifndef AIDE_MOCK_KEYMAP_PAGE_WIDGET_HPP
#define AIDE_MOCK_KEYMAP_PAGE_WIDGET_HPP

#include <memory>

#include <QWidget>

#include "settings/keymap/keymappagewidgetinterface.hpp"

namespace aide::test
{
    class MockKeyMapPageWidget
        : public QWidget
        , public aide::core::KeyMapPageWidgetInterface
    {
    public:
        explicit MockKeyMapPageWidget(QWidget* parent = nullptr);
        void setTreeModel(std::shared_ptr<QAbstractItemModel> model) override;

        [[nodiscard]] bool wasTreeModelSet() const;
        [[nodiscard]] int numberOfRowsInTree() const;

        void showContextMenu(const core::ContextMenuEntries& entries) override;

        [[nodiscard]] bool wasContextMeuSet() const;
        [[nodiscard]] size_t numberOfContextMenuEntries() const;
        [[nodiscard]] aide::core::ContextMenuItemType contextMenuItemTypeAt(
            size_t index) const;
        [[nodiscard]] std::string contextMenuDisplayTextAt(size_t index) const;

    private:
        std::shared_ptr<QAbstractItemModel> treeModel;

        core::ContextMenuEntries contextMenuEntries;

        bool treeModelWasSet{false};

        bool contextMenuWasSet{false};
        size_t numberOfSetContextMenuEntries{99};
    };
} // namespace aide::test

#endif // AIDE_MOCK_KEYMAP_PAGE_WIDGET_HPP
