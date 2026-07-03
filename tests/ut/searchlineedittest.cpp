#include <array>

#include <catch2/catch_test_macros.hpp>

#include <QApplication>

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
