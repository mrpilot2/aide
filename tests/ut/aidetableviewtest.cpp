#include <array>

#include <catch2/catch_test_macros.hpp>

#include <QApplication>
#include <QSortFilterProxyModel>
#include <QStandardItemModel>

#include "aide/gui/widgets/aidetableview.hpp"

using aide::widgets::AideTableView;

namespace
{
    constexpr int kViewWidth{200};
    constexpr int kViewHeight{100};
} // namespace

TEST_CASE("AideTableView construction", "[AideTableView]")
{
    SECTION("can be constructed with null parent")
    {
        const AideTableView view(nullptr);
        REQUIRE(view.model() == nullptr);
    }

    SECTION("setPlaceHolderText stores the text without crash")
    {
        AideTableView view(nullptr);
        view.setPlaceHolderText("No items found");
    }
}

TEST_CASE("AideTableView paint paths", "[AideTableView]")
{
    SECTION("paints without crash when model is null")
    {
        AideTableView view(nullptr);
        view.resize(kViewWidth, kViewHeight);
        view.show();
        view.repaint();
        QApplication::processEvents();
    }

    SECTION("paints without crash when model is empty")
    {
        AideTableView view(nullptr);
        QStandardItemModel model(0, 2);
        view.setModel(&model);
        view.resize(kViewWidth, kViewHeight);
        view.show();
        view.repaint();
        QApplication::processEvents();
    }

    SECTION("paints without crash when model has rows and columns")
    {
        AideTableView view(nullptr);
        QStandardItemModel model(2, 2);
        model.setItem(0, 0, new QStandardItem("a"));
        model.setItem(0, 1, new QStandardItem("b"));
        model.setItem(1, 0, new QStandardItem("c"));
        model.setItem(1, 1, new QStandardItem("d"));
        view.setModel(&model);
        view.resize(kViewWidth, kViewHeight);
        view.show();
        view.repaint();
        QApplication::processEvents();
    }

    SECTION("paints without crash when filter hides all rows")
    {
        AideTableView view(nullptr);
        QStandardItemModel source(2, 1);
        source.setItem(0, 0, new QStandardItem("alpha"));
        source.setItem(1, 0, new QStandardItem("beta"));
        QSortFilterProxyModel proxy;
        proxy.setSourceModel(&source);
        proxy.setFilterFixedString("zzz_no_match");
        view.setModel(&proxy);
        view.resize(kViewWidth, kViewHeight);
        view.show();
        view.repaint();
        QApplication::processEvents();
    }

    SECTION("paints without crash with custom placeholder text")
    {
        AideTableView view(nullptr);
        view.setPlaceHolderText("Empty table");
        view.resize(kViewWidth, kViewHeight);
        view.show();
        view.repaint();
        QApplication::processEvents();
    }
}
