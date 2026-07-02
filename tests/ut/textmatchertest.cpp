#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <QString>
#include <QStringList>

#include "settings/textmatcher.hpp"

using aide::core::TextMatcher;
using Catch::Matchers::WithinAbs;

namespace
{
    constexpr auto tolerance = 0.0001;

    // "font" (4 chars) covers "Font:" (5 chars).
    constexpr auto kFontInFontColon = 0.8;
    // "font" (4 chars) buried in "Change font size" (16 chars).
    constexpr auto kFontBuried = 0.25;
    // best-per-word sum: "font" 0.8 + "size" 0.25.
    constexpr auto kFontSizeSum = 1.05;
} // namespace

TEST_CASE("Scoring a single word against a text")
{
    SECTION("scores a full match as 1.0")
    {
        REQUIRE_THAT(TextMatcher::score("font", "font"),
                     WithinAbs(1.0, tolerance));
    }

    SECTION("scores by the fraction of the text the match covers")
    {
        // "font" (4 chars) inside "Font:" (5 chars) -> 0.8
        REQUIRE_THAT(TextMatcher::score("Font:", "font"),
                     WithinAbs(kFontInFontColon, tolerance));
    }

    SECTION("scores a word buried in a long text low")
    {
        // "font" (4 chars) inside "Change font size" (16 chars) -> 0.25
        REQUIRE_THAT(TextMatcher::score("Change font size", "font"),
                     WithinAbs(kFontBuried, tolerance));
    }

    SECTION("scores a non-matching word as 0")
    {
        REQUIRE_THAT(TextMatcher::score("Font:", "colour"),
                     WithinAbs(0.0, tolerance));
    }

    SECTION("matches case-insensitively")
    {
        REQUIRE_THAT(TextMatcher::score("FONT:", "font"),
                     WithinAbs(kFontInFontColon, tolerance));
    }

    SECTION("scores against an empty text as 0")
    {
        REQUIRE_THAT(TextMatcher::score("", "font"), WithinAbs(0.0, tolerance));
    }
}

TEST_CASE("Scoring a set of texts against a set of words")
{
    SECTION("takes the best score across the texts for a single word")
    {
        // "font" scores 0.25 against the long text but 0.8 against "Font:";
        // the best (0.8) is used.
        const QStringList texts{"Change font size", "Font:"};

        REQUIRE_THAT(TextMatcher::score(texts, QStringList{"font"}),
                     WithinAbs(kFontInFontColon, tolerance));
    }

    SECTION("sums the best per-word scores across the words")
    {
        // "font" -> 0.8 against "Font:", "size" (4 chars) -> 0.25 against
        // "Change font size" (16 chars); total 1.05.
        const QStringList texts{"Change font size", "Font:"};

        REQUIRE_THAT(TextMatcher::score(texts, QStringList{"font", "size"}),
                     WithinAbs(kFontSizeSum, tolerance));
    }

    SECTION("contributes 0 for a word that matches no text")
    {
        const QStringList texts{"Font:"};

        REQUIRE_THAT(
            TextMatcher::score(texts, QStringList{"font", "nonexistent"}),
            WithinAbs(kFontInFontColon, tolerance));
    }

    SECTION("scores as 0 when there are no words")
    {
        const QStringList texts{"Font:"};

        REQUIRE_THAT(TextMatcher::score(texts, QStringList{}),
                     WithinAbs(0.0, tolerance));
    }
}
