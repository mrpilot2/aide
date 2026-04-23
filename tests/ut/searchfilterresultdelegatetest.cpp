#include <array>

#include <catch2/catch_test_macros.hpp>

#include <QApplication>
#include <QPainter>
#include <QPixmap>
#include <QStandardItemModel>
#include <QStyleOptionViewItem>

#include "widgets/multicolumnsortfilterproxymodel.hpp"
#include "widgets/searchfilterresultdelegate.hpp"

using aide::widgets::MultiColumnSortFilterProxyModel;
using aide::widgets::SearchFilterResultDelegate;

namespace
{
    constexpr int kPixmapWidth{200};
    constexpr int kPixmapHeight{20};
    constexpr int kOptionWidth{100};
    constexpr int kOptionHeight{20};

    class TestableDelegate : public SearchFilterResultDelegate
    {
    public:
        using SearchFilterResultDelegate::SearchFilterResultDelegate;

        void callPaint(QPainter* painter, const QStyleOptionViewItem& option,
                       const QModelIndex& index) const
        {
            paint(painter, option, index);
        }
    };

    QStyleOptionViewItem makeOption()
    {
        QStyleOptionViewItem option;
        option.rect = QRect(0, 0, kOptionWidth, kOptionHeight);
        return option;
    }
} // namespace

TEST_CASE("SearchFilterResultDelegate paint", "[SearchFilterResultDelegate]")
{
    // NOLINTNEXTLINE
    std::array<char*, 1> appName{{const_cast<char*>("aide_test")}};
    int numberOfArgs{1};
    const QApplication app(numberOfArgs, appName.data());

    QPixmap pixmap(kPixmapWidth, kPixmapHeight);
    QPainter painter(&pixmap);
    const QStyleOptionViewItem option{makeOption()};
    const TestableDelegate delegate;

    SECTION("paint with invalid index returns early without crash")
    {
        delegate.callPaint(&painter, option, QModelIndex());
    }

    SECTION("paint with valid index on plain model uses QItemDelegate fallback")
    {
        QStandardItemModel model;
        model.appendRow(new QStandardItem("apple"));
        const QModelIndex index = model.index(0, 0);

        delegate.callPaint(&painter, option, index);
    }

    SECTION("paint with valid index on filter model with no filter set")
    {
        QStandardItemModel source;
        source.appendRow(
            {new QStandardItem("apple"), new QStandardItem("red")});

        MultiColumnSortFilterProxyModel proxy;
        proxy.setSourceModel(&source);
        const QModelIndex index = proxy.index(0, 0);

        delegate.callPaint(&painter, option, index);
    }

    SECTION("paint with matching filter highlights without crash")
    {
        QStandardItemModel source;
        source.appendRow(
            {new QStandardItem("apple"), new QStandardItem("red")});
        source.appendRow(
            {new QStandardItem("banana"), new QStandardItem("yellow")});

        MultiColumnSortFilterProxyModel proxy;
        proxy.setSourceModel(&source);
        proxy.setFilterForColumn(0, "app");
        const QModelIndex index = proxy.index(0, 0);

        delegate.callPaint(&painter, option, index);
    }

    SECTION("paint with all-column filter and match does not crash")
    {
        QStandardItemModel source;
        source.appendRow(
            {new QStandardItem("cherry"), new QStandardItem("red")});

        MultiColumnSortFilterProxyModel proxy;
        proxy.setSourceModel(&source);
        proxy.setFilterForColumn(-1, "red");
        const QModelIndex index = proxy.index(0, 0);

        delegate.callPaint(&painter, option, index);
    }

    SECTION("paint with non-matching filter does not crash")
    {
        QStandardItemModel source;
        source.appendRow(new QStandardItem("apple"));

        MultiColumnSortFilterProxyModel proxy;
        proxy.setSourceModel(&source);
        proxy.setFilterForColumn(0, "zzz_no_match");
        // proxy has no visible rows, so paint via the source model index
        const QModelIndex index = source.index(0, 0);

        delegate.callPaint(&painter, option, index);
    }
}
