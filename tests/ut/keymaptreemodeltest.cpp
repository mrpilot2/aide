#include <array>

#include <catch2/catch.hpp>

#include "actionregistry.hpp"
#include "hierarchicalid.hpp"
#include "nulllogger.hpp"
#include "settings/keymap/keymaptreemodel.hpp"

using aide::ActionRegistry;
using aide::ActionRegistryInterfacePtr;
using aide::HierarchicalId;
using aide::core::KeyMapTreeModel;
using aide::test::NullLogger;

TEST_CASE("A new keymap tree model without any action registered")
{
    auto logger = std::make_shared<NullLogger>();
    auto registry(std::make_shared<ActionRegistry>(logger));
    KeyMapTreeModel treeModel(registry);

    SECTION("has no rows") { REQUIRE(treeModel.rowCount() == 0); }

    SECTION("has two columns") { REQUIRE(treeModel.columnCount() == 2); }
}

TEST_CASE("A new keymap tree model with one action registered")
{
    auto logger = std::make_shared<NullLogger>();
    auto registry(std::make_shared<ActionRegistry>(logger));

    std::shared_ptr<QAction> action{std::make_shared<QAction>()};
    registry->registerAction(action, HierarchicalId("New File"));
    KeyMapTreeModel treeModel(registry);

    SECTION("has one row") { REQUIRE(treeModel.rowCount() == 1); }

    SECTION("has two columns") { REQUIRE(treeModel.columnCount() == 2); }

    SECTION("provides action id in first column and row")
    {
        QModelIndex index(treeModel.index(0, 0));
        REQUIRE(
            treeModel.data(index, Qt::DisplayRole).toString().toStdString() ==
            "New File");
    }

    SECTION("does not have parent in first row")
    {
        QModelIndex index(treeModel.index(0, 0));

        auto parentIndex = treeModel.parent(index);
        REQUIRE(!parentIndex.isValid());
    }
}

TEST_CASE("A new keymap tree model with multiple actions registered")
{
    auto logger = std::make_shared<NullLogger>();
    auto registry(std::make_shared<ActionRegistry>(logger));

    std::shared_ptr<QAction> action{std::make_shared<QAction>()};

    registry->registerAction(action, HierarchicalId("Main Menu")("Close"));
    registry->registerAction(action, HierarchicalId("Main Menu")("New File"));
    KeyMapTreeModel treeModel(registry);

    SECTION("use same tree item for same group")
    {
        REQUIRE(treeModel.rowCount() == 1);
    }

    SECTION("creates a valid child model index")
    {
        auto parentIndex = treeModel.index(0, 0);

        auto childIndex = treeModel.index(0, 0, parentIndex);

        REQUIRE(treeModel.data(childIndex, Qt::DisplayRole)
                    .toString()
                    .toStdString() == "Close");
    }

    SECTION("creates a valid parent index")
    {
        auto parentIndex = treeModel.index(0, 0);

        auto childIndex = treeModel.index(1, 0, parentIndex);

        REQUIRE(treeModel.parent(childIndex) == parentIndex);
    }

    SECTION("returns invalid model index if child does not exist")
    {
        auto parentIndex = treeModel.index(0, 0);

        auto childIndex = treeModel.index(5, 0, parentIndex);

        REQUIRE(!childIndex.isValid());
    }
}

TEST_CASE("Any keymap tree model")
{
    auto logger = std::make_shared<NullLogger>();
    auto registry(std::make_shared<ActionRegistry>(logger));

    std::shared_ptr<QAction> action{std::make_shared<QAction>("abc", nullptr)};
    auto id = HierarchicalId("Main Menu")("Close");
    registry->registerAction(action, id, "Thoughtful description");
    KeyMapTreeModel treeModel(registry);

    SECTION("provides horizontal header")
    {
        REQUIRE(treeModel.headerData(0, Qt::Horizontal, Qt::DisplayRole)
                    .toString()
                    .toStdString() == "Action");
        REQUIRE(treeModel.headerData(1, Qt::Horizontal, Qt::DisplayRole)
                    .toString()
                    .toStdString() == "Shortcuts");
    }

    SECTION("does not provide vertical header")
    {
        REQUIRE(
            !treeModel.headerData(0, Qt::Vertical, Qt::DisplayRole).isValid());
    }

    SECTION("has selectable items")
    {
        QModelIndex index = treeModel.index(0, 0);

        REQUIRE(treeModel.flags(index).testFlag(Qt::ItemIsSelectable));
    }

    SECTION("aligns shortcuts header on right side")
    {
        REQUIRE(treeModel.headerData(1, Qt::Horizontal,
                                     Qt::TextAlignmentRole) == Qt::AlignRight);
    }

    SECTION("aligns shortcuts on right side")
    {
        QModelIndex index = treeModel.index(0, 1);

        REQUIRE(index.data(Qt::TextAlignmentRole) == Qt::AlignRight);
    }

    SECTION("shows action description as tooltip")
    {
        QModelIndex root = treeModel.index(0, 0, QModelIndex());
        QModelIndex elem = treeModel.index(0, 0, root);

        REQUIRE(elem.data(Qt::ToolTipRole).toString().toStdString() ==
                "Thoughtful description");
    }

    SECTION("does not crash if no tooltip is defined for current index")
    {
        QModelIndex root = treeModel.index(0, 0, QModelIndex());

        REQUIRE(root.data(Qt::ToolTipRole) == QVariant());
    }

    SECTION("returns empty QVariant for unsupported role")
    {
        QModelIndex index = treeModel.index(0, 1);

        REQUIRE(treeModel.data(index, Qt::CheckStateRole) == QVariant());
    }
}
