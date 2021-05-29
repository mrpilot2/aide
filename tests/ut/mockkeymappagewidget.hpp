
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

    private:
        std::shared_ptr<QAbstractItemModel> treeModel;

        bool treeModelWasSet{false};
    };
} // namespace aide::test

#endif // AIDE_MOCK_KEYMAP_PAGE_WIDGET_HPP
