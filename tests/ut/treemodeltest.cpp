#include <memory>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include <QVariant>

#include "treeitem.hpp"
#include "treemodel.hpp"

using aide::core::TreeItem;
using aide::core::TreeItemPtr;
using aide::core::TreeModel;

namespace
{
    // TreeModel leaves QAbstractItemModel::data() unimplemented so
    // subclasses can define their own column semantics; this minimal
    // subclass exists only so the base traversal logic can be tested.
    class ConcreteTreeModel : public TreeModel
    {
    public:
        using TreeModel::TreeModel;

        [[nodiscard]] QVariant data(const QModelIndex& index,
                                    const int role) const override
        {
            if (!index.isValid() || role != Qt::DisplayRole) { return {}; }

            return static_cast<TreeItem*>(index.internalPointer())
                ->data(static_cast<size_t>(index.column()));
        }
    };

    struct Tree
    {
        TreeItemPtr root{
            std::make_shared<TreeItem>(std::vector<QVariant>({"Header"}))};
        TreeItemPtr child1{std::make_shared<TreeItem>(
            std::vector<QVariant>({"child1"}), root)};
        TreeItemPtr child2{std::make_shared<TreeItem>(
            std::vector<QVariant>({"child2"}), root)};
        TreeItemPtr grandchild{std::make_shared<TreeItem>(
            std::vector<QVariant>({"grandchild"}), child1)};

        Tree()
        {
            root->appendChild(child1);
            root->appendChild(child2);
            child1->appendChild(grandchild);
        }
    };
} // namespace

TEST_CASE("A tree model built on top of a two-level tree item hierarchy")
{
    const Tree tree;
    const ConcreteTreeModel model(nullptr, tree.root);

    SECTION("reports the root's child count as the top-level row count")
    {
        REQUIRE(model.rowCount(QModelIndex()) == 2);
    }

    SECTION("reports a child item's own child count as its row count")
    {
        const QModelIndex child1Index = model.index(0, 0);
        REQUIRE(model.rowCount(child1Index) == 1);
    }

    SECTION("reports the root's column count as the top-level column count")
    {
        REQUIRE(model.columnCount(QModelIndex()) == 1);
    }

    SECTION("creates a valid index for each top-level row")
    {
        const QModelIndex child1Index = model.index(0, 0);
        const QModelIndex child2Index = model.index(1, 0);

        REQUIRE(child1Index.isValid());
        REQUIRE(child2Index.isValid());
        REQUIRE(static_cast<TreeItem*>(child1Index.internalPointer()) ==
                tree.child1.get());
        REQUIRE(static_cast<TreeItem*>(child2Index.internalPointer()) ==
                tree.child2.get());
    }

    SECTION("creates an invalid index for an out-of-range row")
    {
        REQUIRE_FALSE(model.index(99, 0).isValid());
    }

    SECTION("creates a valid index for a grandchild via its parent index")
    {
        const QModelIndex child1Index     = model.index(0, 0);
        const QModelIndex grandchildIndex = model.index(0, 0, child1Index);

        REQUIRE(grandchildIndex.isValid());
        REQUIRE(static_cast<TreeItem*>(grandchildIndex.internalPointer()) ==
                tree.grandchild.get());
    }

    SECTION("resolves a grandchild's parent index back to its parent item")
    {
        const QModelIndex child1Index     = model.index(0, 0);
        const QModelIndex grandchildIndex = model.index(0, 0, child1Index);

        REQUIRE(model.parent(grandchildIndex) == child1Index);
    }

    SECTION("resolves a top-level item's parent index as invalid")
    {
        const QModelIndex child1Index = model.index(0, 0);
        REQUIRE_FALSE(model.parent(child1Index).isValid());
    }

    SECTION("resolves an invalid index's parent as invalid")
    {
        REQUIRE_FALSE(model.parent(QModelIndex()).isValid());
    }

    SECTION("exposes the root item's data as horizontal header data")
    {
        REQUIRE(model.headerData(0, Qt::Horizontal, Qt::DisplayRole)
                    .toString()
                    .toStdString() == "Header");
    }

    SECTION("returns no vertical header data")
    {
        REQUIRE(model.headerData(0, Qt::Vertical, Qt::DisplayRole).isValid() ==
                false);
    }
}
