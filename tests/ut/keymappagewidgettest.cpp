#include <array>
#include <memory>

#include <catch2/catch_test_macros.hpp>

#include <QApplication>
#include <QStandardItemModel>
#include <QTimer>
#include <QTreeView>

#include "settings/keymap/keymappagewidget.hpp"
#include "settings/keymap/keymappagewidgetinterface.hpp"

using aide::core::ContextMenuEntries;
using aide::core::ContextMenuEntry;
using aide::core::ContextMenuItemType;
using aide::gui::KeymapPageWidget;

TEST_CASE("KeymapPageWidget construction", "[KeymapPageWidget]")
{
    // NOLINTNEXTLINE
    std::array<char*, 1> appName{{const_cast<char*>("aide_test")}};
    int numberOfArgs{1};
    const QApplication app(numberOfArgs, appName.data());

    SECTION("can be constructed with null parent")
    {
        const KeymapPageWidget widget;
        REQUIRE(widget.findChild<QTreeView*>("treeView") != nullptr);
    }
}

TEST_CASE("KeymapPageWidget setTreeModel", "[KeymapPageWidget]")
{
    // NOLINTNEXTLINE
    std::array<char*, 1> appName{{const_cast<char*>("aide_test")}};
    int numberOfArgs{1};
    const QApplication app(numberOfArgs, appName.data());

    KeymapPageWidget widget;
    const auto* treeView = widget.findChild<QTreeView*>("treeView");
    REQUIRE(treeView != nullptr);

    SECTION("sets the tree view's model to the given model")
    {
        auto model = std::make_shared<QStandardItemModel>();
        widget.setTreeModel(model);

        REQUIRE(treeView->model() == model.get());
    }

    SECTION("setting a second model replaces the first")
    {
        auto first  = std::make_shared<QStandardItemModel>();
        auto second = std::make_shared<QStandardItemModel>();
        widget.setTreeModel(first);
        widget.setTreeModel(second);

        REQUIRE(treeView->model() == second.get());
    }
}

TEST_CASE("KeymapPageWidget showContextMenu", "[KeymapPageWidget]")
{
    // NOLINTNEXTLINE
    std::array<char*, 1> appName{{const_cast<char*>("aide_test")}};
    int numberOfArgs{1};
    const QApplication app(numberOfArgs, appName.data());

    KeymapPageWidget widget;
    widget.show();
    QApplication::processEvents();

    SECTION("showContextMenu with empty entries does not crash")
    {
        // Schedule a close of the blocking exec() call before it opens
        QTimer::singleShot(0, []() {
            auto* popup = QApplication::activePopupWidget();
            if (popup != nullptr) { popup->close(); }
        });
        widget.showContextMenu({});
    }

    SECTION("showContextMenu with separator entry does not crash")
    {
        ContextMenuEntry sep;
        sep.type        = ContextMenuItemType::SEPARATOR;
        sep.displayText = "";

        QTimer::singleShot(0, []() {
            auto* popup = QApplication::activePopupWidget();
            if (popup != nullptr) { popup->close(); }
        });
        widget.showContextMenu({sep});
    }
}
