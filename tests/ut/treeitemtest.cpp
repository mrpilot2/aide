#include <memory>
#include <vector>

#include <catch2/catch.hpp>

#include "treeitem.hpp"

using aide::core::TreeItem;
using aide::core::TreeItemPtr;

TEST_CASE("A new root tree item")
{
    TreeItem item({"abc", 1, false});

    SECTION("has column count according to item data")
    {
        REQUIRE(item.columnCount() == 3);
    }

    SECTION("is in row zero")
    {
        REQUIRE(item.row() == 0);
    }

    SECTION("has no parent")
    {
        REQUIRE(item.parent().lock() == nullptr);
    }

    SECTION("has no children")
    {
        REQUIRE(item.childCount() == 0);
        REQUIRE(item.child(0) == nullptr);
    }

    SECTION("has data in all columns")
    {
        REQUIRE(item.data(0).toString().toStdString() == "abc");
        REQUIRE(item.data(1).toInt() == 1);
        REQUIRE(item.data(2).toBool() == false);
    }
}

TEST_CASE("First new child tree item")
{
    auto parent =
        std::make_shared<TreeItem>(std::vector<QVariant>({"abc"}), nullptr);
    auto child =
        std::make_shared<TreeItem>(std::vector<QVariant>({{"def", 2}}), parent);
    parent->appendChild(child);

    SECTION("has column count according to item data")
    {
        REQUIRE(child->columnCount() == 2);
    }

    SECTION("is in row zero ")
    {
        REQUIRE(child->row() == 0);
    }

    SECTION("has parent")
    {
        REQUIRE(child->parent().lock() != nullptr);
        REQUIRE(child->parent().lock()->data(0).toString().toStdString() ==
                "abc");
    }

    SECTION("has no children")
    {
        REQUIRE(child->childCount() == 0);
        REQUIRE(child->child(0) == nullptr);
    }

    SECTION("has data in all columns")
    {
        REQUIRE(child->data(0).toString().toStdString() == "def");
        REQUIRE(child->data(1).toInt() == 2);
    }

    SECTION("can be accessed from parent item")
    {
        REQUIRE(parent->child(0) == child);
    }
}

TEST_CASE("Second new child item")
{
    auto parent =
        std::make_shared<TreeItem>(std::vector<QVariant>({"abc"}), nullptr);
    auto child1 =
        std::make_shared<TreeItem>(std::vector<QVariant>({{"def", 2}}), parent);
    auto child2 =
        std::make_shared<TreeItem>(std::vector<QVariant>({3, true}), parent);
    parent->appendChild(child1);
    parent->appendChild(child2);

    SECTION("is in row one")
    {
        REQUIRE(child2->row() == 1);
    }

    SECTION("has same parent as first child")
    {
        REQUIRE(child2->parent().lock() == child1->parent().lock());
    }

    SECTION("can be accessed from parent item")
    {
        REQUIRE(parent->child(1) == child2);
    }
}

TEST_CASE("Any tree item")
{
    auto parent =
        std::make_shared<TreeItem>(std::vector<QVariant>({"abc"}), nullptr);
    auto child1 =
        std::make_shared<TreeItem>(std::vector<QVariant>({{"def", 2}}), parent);
    auto child2 =
        std::make_shared<TreeItem>(std::vector<QVariant>({3, true}), parent);
    parent->appendChild(child1);
    parent->appendChild(child2);

    SECTION("allows to set new data")
    {
        child2->setData(1, false);
        REQUIRE(!child2->data(1).toBool());
    }
}
