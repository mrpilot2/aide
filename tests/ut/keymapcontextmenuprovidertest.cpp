#include <actionregistry.hpp>
#include <array>
#include <hierarchicalid.hpp>

#include <catch2/catch_test_macros.hpp>

#include <QApplication>
#include <QModelIndex>

#include <settings/keymap/keymapcontextmenuprovider.hpp>
#include <settings/keymap/keymaptreemodel.hpp>

#include "mockkeymappagewidget.hpp"
#include "mocksettings.hpp"
#include "nulllogger.hpp"

using aide::ActionRegistry;
using aide::HierarchicalId;
using aide::core::KeymapContextMenuProvider;
using aide::core::KeyMapTreeModel;
using aide::test::MockKeyMapPageWidget;
using aide::test::MockSettings;
using aide::test::NullLogger;

TEST_CASE("Any keymap context menu provider")
{
    int numberOfArgs{1};
    // NOLINTNEXTLINE
    std::array<char*, 1> appName{{const_cast<char*>("aide_test")}};

    QApplication app{numberOfArgs, appName.data()};

    auto logger = std::make_shared<NullLogger>();
    MockSettings settings;
    auto registry(std::make_shared<ActionRegistry>(settings, logger));

    auto widget = std::make_shared<MockKeyMapPageWidget>();

    SECTION("does not provide a context menu if model index is invalid")
    {
        KeyMapTreeModel treeModel(registry);
        QModelIndex index(treeModel.index(-3, -1, QModelIndex()));

        KeymapContextMenuProvider provider{&treeModel, widget.get()};

        provider.createAndShowContextMenu(index);

        REQUIRE(!widget->wasContextMeuSet());
    }

    SECTION("does not provide a menu if no action is assigned to tree item")
    {
        auto action{std::make_shared<QAction>()};

        registry->registerAction(
            action, HierarchicalId("Main Menu")("File")("New File"));

        KeyMapTreeModel treeModel(registry);
        QModelIndex index(treeModel.index(0, 0, QModelIndex()));

        KeymapContextMenuProvider provider{&treeModel, widget.get()};
        provider.createAndShowContextMenu(index);

        REQUIRE(!widget->wasContextMeuSet());
    }

    SECTION(
        "provides only add shortcut action if selected index is valid and "
        "action is assigned but no default action or user shortcut is assigned")
    {
        auto action{std::make_shared<QAction>()};

        registry->registerAction(
            action, HierarchicalId("Main Menu")("File")("New File"));

        KeyMapTreeModel treeModel(registry);

        QModelIndex index(treeModel.index(
            0, 0, treeModel.index(0, 0, treeModel.index(0, 0, QModelIndex()))));

        KeymapContextMenuProvider provider{&treeModel, widget.get()};
        provider.createAndShowContextMenu(index);

        REQUIRE(widget->wasContextMeuSet());
        REQUIRE(widget->numberOfContextMenuEntries() == 1);
        REQUIRE(widget->contextMenuItemTypeAt(0) ==
                aide::core::ContextMenuItemType::ADD_KEYBOARD_SHORTCUT);
        REQUIRE(widget->contextMenuDisplayTextAt(0) == "Add keyboard shortcut");
    }

    SECTION(
        "provides remove shortcut action if action is assigned with one "
        "shortcut")
    {
        auto action{std::make_shared<QAction>()};

        registry->registerAction(
            action, HierarchicalId("Main Menu")("File")("New File"),
            {QKeySequence("Alt+F4")});

        KeyMapTreeModel treeModel(registry);

        QModelIndex index(treeModel.index(
            0, 0, treeModel.index(0, 0, treeModel.index(0, 0, QModelIndex()))));

        KeymapContextMenuProvider provider{&treeModel, widget.get()};
        provider.createAndShowContextMenu(index);

        REQUIRE(widget->wasContextMeuSet());
        REQUIRE(widget->numberOfContextMenuEntries() == 3);
        REQUIRE(widget->contextMenuItemTypeAt(0) ==
                aide::core::ContextMenuItemType::ADD_KEYBOARD_SHORTCUT);
        REQUIRE(widget->contextMenuItemTypeAt(1) ==
                aide::core::ContextMenuItemType::SEPARATOR);
        REQUIRE(widget->contextMenuItemTypeAt(2) ==
                aide::core::ContextMenuItemType::REMOVE_SHORTCUT);
        REQUIRE(widget->contextMenuDisplayTextAt(2) == "Remove Alt+F4");
    }

    SECTION(
        "provides remove shortcut action for every shortcut assigned to action")
    {
        auto action{std::make_shared<QAction>()};

        registry->registerAction(
            action, HierarchicalId("Main Menu")("File")("New File"),
            {QKeySequence("Alt+F4"), QKeySequence{"Ctrl+Q"}});

        KeyMapTreeModel treeModel(registry);

        QModelIndex index(treeModel.index(
            0, 0, treeModel.index(0, 0, treeModel.index(0, 0, QModelIndex()))));

        KeymapContextMenuProvider provider{&treeModel, widget.get()};
        provider.createAndShowContextMenu(index);

        REQUIRE(widget->wasContextMeuSet());
        REQUIRE(widget->numberOfContextMenuEntries() == 4);
        REQUIRE(widget->contextMenuItemTypeAt(3) ==
                aide::core::ContextMenuItemType::REMOVE_SHORTCUT);
        REQUIRE(widget->contextMenuDisplayTextAt(3) == "Remove Ctrl+Q");
    }

    SECTION(
        "provides reset to defaults action if custom shortcuts are assigned to "
        "current action")
    {
        const auto settingsKey{
            HierarchicalId("Keymap")("Main Menu")("File")("New File")};

        settings.setValue(settingsKey, "Shift+5");

        auto action{std::make_shared<QAction>()};

        registry->registerAction(
            action, HierarchicalId("Main Menu")("File")("New File"),
            {QKeySequence("Alt+F4"), QKeySequence{"Ctrl+Q"}});

        KeyMapTreeModel treeModel(registry);

        QModelIndex index(treeModel.index(
            0, 0, treeModel.index(0, 0, treeModel.index(0, 0, QModelIndex()))));

        KeymapContextMenuProvider provider{&treeModel, widget.get()};
        provider.createAndShowContextMenu(index);

        REQUIRE(widget->wasContextMeuSet());
        REQUIRE(widget->numberOfContextMenuEntries() == 5);
        REQUIRE(widget->contextMenuItemTypeAt(2) ==
                aide::core::ContextMenuItemType::REMOVE_SHORTCUT);
        REQUIRE(widget->contextMenuDisplayTextAt(2) == "Remove Shift+5");

        REQUIRE(widget->contextMenuItemTypeAt(3) ==
                aide::core::ContextMenuItemType::SEPARATOR);
        REQUIRE(widget->contextMenuItemTypeAt(4) ==
                aide::core::ContextMenuItemType::RESET_TO_DEFAULTS);
        REQUIRE(widget->contextMenuDisplayTextAt(4) == "Reset to default");
    }
}
