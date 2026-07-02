#include <catch2/catch_test_macros.hpp>

#include <QStringList>

#include "mocksettings.hpp"
#include "settings/searchhistory.hpp"

using aide::core::SearchHistory;
using aide::test::MockSettings;

TEST_CASE("A settings search history")
{
    MockSettings settings;
    SearchHistory history{settings};

    SECTION("adds a committed pattern to the front")
    {
        history.commit("font");

        REQUIRE(history.entries() == QStringList{"font"});
    }

    SECTION("keeps the most recently committed pattern first")
    {
        history.commit("font");
        history.commit("color");

        REQUIRE(history.entries() == QStringList{"color", "font"});
    }

    SECTION("moves a repeated pattern to the front instead of duplicating it")
    {
        history.commit("font");
        history.commit("color");
        history.commit("font");

        REQUIRE(history.entries() == QStringList{"font", "color"});
    }

    SECTION("caps the history at five entries, dropping the oldest")
    {
        history.commit("one");
        history.commit("two");
        history.commit("three");
        history.commit("four");
        history.commit("five");
        history.commit("six");

        REQUIRE(history.entries() ==
                QStringList{"six", "five", "four", "three", "two"});
    }

    SECTION("persists through the settings abstraction across instances")
    {
        history.commit("font");
        history.commit("color");

        const SearchHistory reloaded{settings};

        REQUIRE(reloaded.entries() == QStringList{"color", "font"});
    }

    SECTION("ignores an empty pattern")
    {
        history.commit("font");
        history.commit("");

        REQUIRE(history.entries() == QStringList{"font"});
    }
}
