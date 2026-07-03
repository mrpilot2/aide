#include <array>

#include <catch2/catch_test_macros.hpp>

#include <QApplication>
#include <QSortFilterProxyModel>
#include <QStringListModel>

#include "aide/gui/widgets/aidetreeview.hpp"

using aide::widgets::AideTreeView;

namespace
{
    constexpr int kViewWidth{200};
    constexpr int kViewHeight{100};
} // namespace

TEST_CASE("AideTreeView construction", "[AideTreeView]")
{
    SECTION("can be constructed with null parent")
    {
        const AideTreeView view(nullptr);
        REQUIRE(view.model() == nullptr);
    }

    SECTION("setPlaceHolderText stores the text without crash")
    {
        AideTreeView view(nullptr);
        view.setPlaceHolderText("Nothing here");
    }
}

TEST_CASE("AideTreeView paint paths", "[AideTreeView]")
{
    SECTION("paints without crash when model is null")
    {
        AideTreeView view(nullptr);
        view.resize(kViewWidth, kViewHeight);
        view.show();
        view.repaint();
        QApplication::processEvents();
    }

    SECTION("paints without crash when model is empty")
    {
        AideTreeView view(nullptr);
        QStringListModel model;
        view.setModel(&model);
        view.resize(kViewWidth, kViewHeight);
        view.show();
        view.repaint();
        QApplication::processEvents();
    }

    SECTION("paints without crash when model has rows (normal paint path)")
    {
        AideTreeView view(nullptr);
        QStringListModel model({"item one", "item two"});
        view.setModel(&model);
        view.resize(kViewWidth, kViewHeight);
        view.show();
        view.repaint();
        QApplication::processEvents();
    }

    SECTION("paints without crash when filter hides all rows")
    {
        AideTreeView view(nullptr);
        QStringListModel source({"apple", "banana"});
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
        AideTreeView view(nullptr);
        view.setPlaceHolderText("No data available");
        view.resize(kViewWidth, kViewHeight);
        view.show();
        view.repaint();
        QApplication::processEvents();
    }
}
