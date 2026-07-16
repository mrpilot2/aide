#include <string>

#include <catch2/catch_test_macros.hpp>

#include "desktopentrynameresolver.hpp"

using aide::core::DesktopEntryNameResolver;

namespace
{
    constexpr auto kDolphinDesktopEntry =
        "[Desktop Entry]\n"
        "Type=Application\n"
        "Name=Dolphin\n"
        "Name[de]=Dolphin\n"
        "Name[de_AT]=Dolphin (AT)\n"
        "GenericName=File Manager\n"
        "GenericName[de]=Dateimanager\n"
        "\n"
        "[Desktop Action NewWindow]\n"
        "Name=New Window\n"
        "Name[de]=Neues Fenster\n";
} // namespace

TEST_CASE("Resolving a display name from .desktop file content for a locale")
{
    SECTION("prefers an exact language+region match")
    {
        REQUIRE(DesktopEntryNameResolver::resolve(kDolphinDesktopEntry,
                                                  "de_AT") == "Dolphin (AT)");
    }

    SECTION("falls back to the language-only match when no region matches")
    {
        REQUIRE(DesktopEntryNameResolver::resolve(kDolphinDesktopEntry,
                                                  "de_DE") == "Dolphin");
    }

    SECTION("falls back to the untranslated default when no locale matches")
    {
        REQUIRE(DesktopEntryNameResolver::resolve(kDolphinDesktopEntry,
                                                  "fr_FR") == "Dolphin");
    }

    SECTION("ignores localized names outside the [Desktop Entry] section")
    {
        const auto resolved =
            DesktopEntryNameResolver::resolve(kDolphinDesktopEntry, "de_DE");
        REQUIRE(resolved != "Neues Fenster");
    }

    SECTION("returns nullopt when no Name entry is present at all")
    {
        constexpr auto kNoNameEntry =
            "[Desktop Entry]\n"
            "Type=Application\n"
            "Exec=nautilus\n";
        REQUIRE_FALSE(DesktopEntryNameResolver::resolve(kNoNameEntry, "de_DE")
                          .has_value());
    }

    SECTION("matches a bare language locale directly")
    {
        REQUIRE(DesktopEntryNameResolver::resolve(kDolphinDesktopEntry, "de") ==
                "Dolphin");
    }
}
