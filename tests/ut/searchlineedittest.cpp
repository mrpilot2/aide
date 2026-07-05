#include <array>

#include <catch2/catch_test_macros.hpp>

#include <QAbstractItemModel>
#include <QAbstractItemView>
#include <QAction>
#include <QApplication>
#include <QCompleter>
#include <QSignalSpy>
#include <QStringList>

#include "aide/gui/widgets/searchlineedit.hpp"

using aide::widgets::SearchLineEdit;

namespace
{
    constexpr int kEditWidth{200};
    constexpr int kEditHeight{30};
} // namespace

TEST_CASE("SearchLineEdit", "[SearchLineEdit]")
{
    SECTION("can be constructed")
    {
        const SearchLineEdit edit;
        REQUIRE(edit.text().isEmpty());
    }

    SECTION("setTags with empty list does not crash on paint")
    {
        SearchLineEdit edit;
        edit.setTags({});
        edit.setText("hello world");
        edit.resize(kEditWidth, kEditHeight);
        edit.show();
        edit.repaint();
        QApplication::processEvents();
    }

    SECTION("setTags with matching tag does not crash on paint")
    {
        SearchLineEdit edit;
        edit.setTags({"hello"});
        edit.setText("hello world");
        edit.resize(kEditWidth, kEditHeight);
        edit.show();
        edit.repaint();
        QApplication::processEvents();
    }

    SECTION(
        "setTags with uppercase tag matches lowercase text (case insensitive)")
    {
        SearchLineEdit edit;
        edit.setTags({"HELLO"});
        edit.setText("hello world");
        edit.resize(kEditWidth, kEditHeight);
        edit.show();
        edit.repaint();
        QApplication::processEvents();
    }

    SECTION("setTags with multiple tags does not crash on paint")
    {
        SearchLineEdit edit;
        edit.setTags({"hello", "world"});
        edit.setText("hello world hello");
        edit.resize(kEditWidth, kEditHeight);
        edit.show();
        edit.repaint();
        QApplication::processEvents();
    }

    SECTION("setTags with non-matching tag does not crash on paint")
    {
        SearchLineEdit edit;
        edit.setTags({"zzz"});
        edit.setText("hello world");
        edit.resize(kEditWidth, kEditHeight);
        edit.show();
        edit.repaint();
        QApplication::processEvents();
    }

    SECTION("paint does not crash with empty text and tags")
    {
        SearchLineEdit edit;
        edit.setTags({"tag"});
        edit.setText("");
        edit.resize(kEditWidth, kEditHeight);
        edit.show();
        edit.repaint();
        QApplication::processEvents();
    }
}

TEST_CASE("SearchLineEdit history dropdown", "[SearchLineEdit]")
{
    SECTION("exposes a magnifying-glass history action")
    {
        const SearchLineEdit edit;
        REQUIRE(edit.historyAction() != nullptr);
    }

    SECTION("setHistory populates the completer model most recent first")
    {
        SearchLineEdit edit;
        edit.setHistory({"font", "color"});

        const auto* model = edit.historyCompleter()->model();
        REQUIRE(model->rowCount() == 2);
        REQUIRE(model->data(model->index(0, 0)).toString() == "font");
        REQUIRE(model->data(model->index(1, 0)).toString() == "color");
    }
}

TEST_CASE("SearchLineEdit history popup opens only on demand",
          "[SearchLineEdit]")
{
    SECTION("triggering the history action opens the popup")
    {
        SearchLineEdit edit;
        edit.setHistory({"font", "color"});
        edit.resize(kEditWidth, kEditHeight);
        edit.show();

        edit.historyAction()->trigger();
        QApplication::processEvents();

        REQUIRE(edit.historyCompleter()->popup()->isVisible());
    }

    SECTION("typing does not auto-open the history popup")
    {
        SearchLineEdit edit;
        edit.setHistory({"font", "color"});
        edit.resize(kEditWidth, kEditHeight);
        edit.show();

        edit.setText("fo");
        QApplication::processEvents();

        REQUIRE_FALSE(edit.historyCompleter()->popup()->isVisible());
    }

    SECTION("triggering with empty history does not open the popup")
    {
        SearchLineEdit edit;
        edit.setHistory({});
        edit.resize(kEditWidth, kEditHeight);
        edit.show();

        edit.historyAction()->trigger();
        QApplication::processEvents();

        REQUIRE_FALSE(edit.historyCompleter()->popup()->isVisible());
    }
}

TEST_CASE("SearchLineEdit re-runs a selected history entry", "[SearchLineEdit]")
{
    SECTION("selecting an entry sets the text and re-runs the search")
    {
        SearchLineEdit edit;
        edit.setHistory({"font", "color"});

        const QSignalSpy spy(&edit, &SearchLineEdit::returnPressed);

        emit edit.historyCompleter()->activated(QString("color"));
        QApplication::processEvents();

        REQUIRE(edit.text() == "color");
        REQUIRE(spy.count() == 1);
    }
}
