#include <array>

#include <catch2/catch_test_macros.hpp>

#include <QStandardItemModel>

#include "widgets/multicolumnsortfilterproxymodel.hpp"

using aide::widgets::MultiColumnSortFilterProxyModel;

namespace
{
    QStandardItemModel* makeTwoColumnModel(QObject* parent = nullptr)
    {
        auto* model = new QStandardItemModel(3, 2, parent);
        model->setItem(0, 0, new QStandardItem("apple"));
        model->setItem(0, 1, new QStandardItem("red"));
        model->setItem(1, 0, new QStandardItem("banana"));
        model->setItem(1, 1, new QStandardItem("yellow"));
        model->setItem(2, 0, new QStandardItem("cherry"));
        model->setItem(2, 1, new QStandardItem("red"));
        return model;
    }
} // namespace

TEST_CASE("A MultiColumnSortFilterProxyModel with no filter set")
{
    QStandardItemModel source;
    source.appendRow({new QStandardItem("foo"), new QStandardItem("bar")});
    source.appendRow({new QStandardItem("baz"), new QStandardItem("qux")});

    MultiColumnSortFilterProxyModel proxy;
    proxy.setSourceModel(&source);

    SECTION("passes all rows through")
    {
        REQUIRE(proxy.rowCount() == 2);
    }

    SECTION("getRegexForColumn returns empty regex for unknown column")
    {
        REQUIRE(proxy.getRegexForColumn(0).pattern().isEmpty());
    }
}

TEST_CASE("A MultiColumnSortFilterProxyModel filtering on a specific column")
{
    MultiColumnSortFilterProxyModel proxy;
    auto* source = makeTwoColumnModel();
    proxy.setSourceModel(source);

    SECTION("shows only rows where column 0 matches the filter")
    {
        proxy.setFilterForColumn(0, "an");

        REQUIRE(proxy.rowCount() == 1);
        REQUIRE(proxy.index(0, 0).data().toString() == "banana");
    }

    SECTION("shows only rows where column 1 matches the filter")
    {
        proxy.setFilterForColumn(1, "red");

        REQUIRE(proxy.rowCount() == 2);
    }

    SECTION("getRegexForColumn returns the set pattern")
    {
        proxy.setFilterForColumn(0, "apple");

        REQUIRE(proxy.getRegexForColumn(0).pattern().contains("apple"));
    }

    SECTION("clearing the filter for that column restores all rows")
    {
        proxy.setFilterForColumn(0, "apple");
        REQUIRE(proxy.rowCount() == 1);

        proxy.clearFilterForColumn(0);

        REQUIRE(proxy.rowCount() == 3);
    }

    SECTION("clearFilterForAllColumns restores all rows")
    {
        proxy.setFilterForColumn(0, "apple");
        proxy.setFilterForColumn(1, "red");
        REQUIRE(proxy.rowCount() == 1);

        proxy.clearFilterForAllColumns();

        REQUIRE(proxy.rowCount() == 3);
    }

    delete source;
}

TEST_CASE(
    "A MultiColumnSortFilterProxyModel with multiple column filters applied")
{
    MultiColumnSortFilterProxyModel proxy;
    auto* source = makeTwoColumnModel();
    proxy.setSourceModel(source);

    SECTION("ANDs filters across columns")
    {
        proxy.setFilterForColumn(0, "cherry");
        proxy.setFilterForColumn(1, "red");

        REQUIRE(proxy.rowCount() == 1);
        REQUIRE(proxy.index(0, 0).data().toString() == "cherry");
    }

    SECTION("returns zero rows when no row satisfies all filters")
    {
        proxy.setFilterForColumn(0, "apple");
        proxy.setFilterForColumn(1, "yellow");

        REQUIRE(proxy.rowCount() == 0);
    }

    delete source;
}

TEST_CASE("A MultiColumnSortFilterProxyModel filtering across all columns")
{
    MultiColumnSortFilterProxyModel proxy;
    auto* source = makeTwoColumnModel();
    proxy.setSourceModel(source);

    SECTION("column -1 filter matches rows where any column matches")
    {
        proxy.setFilterForColumn(-1, "red");

        REQUIRE(proxy.rowCount() == 2);
    }

    SECTION("column -1 filter hides rows where no column matches")
    {
        proxy.setFilterForColumn(-1, "zzz");

        REQUIRE(proxy.rowCount() == 0);
    }

    delete source;
}
