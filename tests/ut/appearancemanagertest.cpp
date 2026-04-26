#include <array>
#include <memory>

#include <catch2/catch_test_macros.hpp>

#include <QApplication>
#include <QIcon>
#include <QSignalSpy>

#include <aide/appearancemanager.hpp>
#include <aide/colorscheme.hpp>
#include <aide/theme.hpp>

#include "mocksettings.hpp"

using aide::AppearanceManager;
using aide::ColorScheme;
using aide::Theme;
using aide::test::MockSettings;

TEST_CASE("AppearanceManager has three built-in themes", "[AppearanceManager]")
{
    int argc{1};
    // NOLINTNEXTLINE
    std::array<char*, 1> appName{{const_cast<char*>("aide_test")}};
    const QApplication app{argc, appName.data()};

    const AppearanceManager manager{std::make_shared<MockSettings>()};

    SECTION("total count is three")
    {
        REQUIRE(manager.themeNames().size() == 3);
    }

    SECTION("System theme is present")
    {
        REQUIRE(manager.themeNames().contains("System"));
    }

    SECTION("Light theme is present")
    {
        REQUIRE(manager.themeNames().contains("Light"));
    }

    SECTION("Dark theme is present")
    {
        REQUIRE(manager.themeNames().contains("Dark"));
    }

    SECTION("default active theme is System")
    {
        REQUIRE(manager.activeThemeName() == "System");
    }
}

TEST_CASE("AppearanceManager registerTheme", "[AppearanceManager]")
{
    int argc{1};
    // NOLINTNEXTLINE
    std::array<char*, 1> appName{{const_cast<char*>("aide_test")}};
    const QApplication app{argc, appName.data()};

    AppearanceManager manager{std::make_shared<MockSettings>()};

    SECTION("succeeds for a new unique theme name")
    {
        const Theme custom{"CustomTheme", QPalette{}, "", {}};
        REQUIRE_NOTHROW(manager.registerTheme(custom));
        REQUIRE(manager.themeNames().contains("CustomTheme"));
    }

    SECTION("throws on duplicate consumer-defined name")
    {
        manager.registerTheme({"CustomTheme", QPalette{}, "", {}});
        REQUIRE_THROWS_AS(
            manager.registerTheme({"CustomTheme", QPalette{}, "", {}}),
            std::invalid_argument);
    }

    SECTION("throws when name matches a built-in theme")
    {
        REQUIRE_THROWS_AS(manager.registerTheme({"Light", QPalette{}, "", {}}),
                          std::invalid_argument);
    }
}

TEST_CASE("AppearanceManager colorScheme", "[AppearanceManager]")
{
    int argc{1};
    // NOLINTNEXTLINE
    std::array<char*, 1> appName{{const_cast<char*>("aide_test")}};
    const QApplication app{argc, appName.data()};

    AppearanceManager manager{std::make_shared<MockSettings>()};

    SECTION("Light theme returns Light")
    {
        manager.applyAppearance("Light", QApplication::font().family(),
                                QApplication::font().pointSize());
        REQUIRE(manager.colorScheme() == ColorScheme::Light);
    }

    SECTION("Dark theme returns Dark")
    {
        manager.applyAppearance("Dark", QApplication::font().family(),
                                QApplication::font().pointSize());
        REQUIRE(manager.colorScheme() == ColorScheme::Dark);
    }

    SECTION("System theme returns a valid color scheme")
    {
        manager.applyAppearance("System", QApplication::font().family(),
                                QApplication::font().pointSize());
        const auto scheme = manager.colorScheme();
        REQUIRE((scheme == ColorScheme::Light || scheme == ColorScheme::Dark));
    }
}

TEST_CASE("AppearanceManager colorSchemeChanged signal", "[AppearanceManager]")
{
    int argc{1};
    // NOLINTNEXTLINE
    std::array<char*, 1> appName{{const_cast<char*>("aide_test")}};
    const QApplication app{argc, appName.data()};

    qRegisterMetaType<aide::ColorScheme>();

    AppearanceManager manager{std::make_shared<MockSettings>()};

    SECTION("is emitted when color scheme changes from Light to Dark")
    {
        manager.applyAppearance("Light", QApplication::font().family(),
                                QApplication::font().pointSize());

        const QSignalSpy spy(&manager,
                             SIGNAL(colorSchemeChanged(aide::ColorScheme)));

        manager.applyAppearance("Dark", QApplication::font().family(),
                                QApplication::font().pointSize());

        REQUIRE(spy.count() == 1);
    }

    SECTION("is not emitted when color scheme stays the same")
    {
        manager.applyAppearance("Light", QApplication::font().family(),
                                QApplication::font().pointSize());

        const QSignalSpy spy(&manager,
                             SIGNAL(colorSchemeChanged(aide::ColorScheme)));

        manager.applyAppearance("Light", QApplication::font().family(),
                                QApplication::font().pointSize());

        REQUIRE(spy.count() == 0);
    }
}

TEST_CASE("AppearanceManager appearanceChanged signal", "[AppearanceManager]")
{
    int argc{1};
    // NOLINTNEXTLINE
    std::array<char*, 1> appName{{const_cast<char*>("aide_test")}};
    const QApplication app{argc, appName.data()};

    AppearanceManager manager{std::make_shared<MockSettings>()};

    SECTION("is emitted on every applyAppearance call")
    {
        const QSignalSpy spy(&manager, SIGNAL(appearanceChanged()));

        manager.applyAppearance("Light", QApplication::font().family(),
                                QApplication::font().pointSize());
        manager.applyAppearance("Light", QApplication::font().family(),
                                QApplication::font().pointSize());

        REQUIRE(spy.count() == 2);
    }
}

TEST_CASE("AppearanceManager settings save and restore", "[AppearanceManager]")
{
    int argc{1};
    // NOLINTNEXTLINE
    std::array<char*, 1> appName{{const_cast<char*>("aide_test")}};
    const QApplication app{argc, appName.data()};

    auto settings = std::make_shared<MockSettings>();

    SECTION("restores theme, font family, and font size from settings")
    {
        constexpr int testFontSize = 14;
        {
            AppearanceManager writer{settings};
            writer.applyAppearance("Dark", "Courier", testFontSize);
        }

        const AppearanceManager reader{settings};
        REQUIRE(reader.activeThemeName() == "Dark");
        REQUIRE(reader.activeFont().family() == "Courier");
        REQUIRE(reader.activeFont().pointSize() == testFontSize);
    }

    SECTION("defaults to System theme when no settings are saved")
    {
        const AppearanceManager manager{std::make_shared<MockSettings>()};
        REQUIRE(manager.activeThemeName() == "System");
    }
}

TEST_CASE("AppearanceManager addIconSearchPath for known theme",
          "[AppearanceManager]")
{
    int argc{1};
    // NOLINTNEXTLINE
    std::array<char*, 1> appName{{const_cast<char*>("aide_test")}};
    const QApplication app{argc, appName.data()};

    AppearanceManager manager{std::make_shared<MockSettings>()};

    REQUIRE_NOTHROW(manager.addIconSearchPath("Light", ":/some/icon/path"));
}

TEST_CASE("AppearanceManager addIconSearchPath for unknown theme",
          "[AppearanceManager]")
{
    int argc{1};
    // NOLINTNEXTLINE
    std::array<char*, 1> appName{{const_cast<char*>("aide_test")}};
    const QApplication app{argc, appName.data()};

    AppearanceManager manager{std::make_shared<MockSettings>()};

    REQUIRE_THROWS_AS(manager.addIconSearchPath("Unknown", ":/some/icon/path"),
                      std::invalid_argument);
}

TEST_CASE("AppearanceManager addIconSearchPath accumulates paths",
          "[AppearanceManager]")
{
    int argc{1};
    // NOLINTNEXTLINE
    std::array<char*, 1> appName{{const_cast<char*>("aide_test")}};
    const QApplication app{argc, appName.data()};

    AppearanceManager manager{std::make_shared<MockSettings>()};
    manager.addIconSearchPath("Light", ":/path/one");
    manager.addIconSearchPath("Light", ":/path/two");

    manager.applyAppearance("Light", QApplication::font().family(),
                            QApplication::font().pointSize());

    const auto paths = QIcon::themeSearchPaths();
    REQUIRE(paths.contains(":/path/one"));
    REQUIRE(paths.contains(":/path/two"));
}

TEST_CASE("AppearanceManager Light theme uses dark icon set",
          "[AppearanceManager]")
{
    int argc{1};
    // NOLINTNEXTLINE
    std::array<char*, 1> appName{{const_cast<char*>("aide_test")}};
    const QApplication app{argc, appName.data()};

    AppearanceManager manager{std::make_shared<MockSettings>()};
    manager.applyAppearance("Light", QApplication::font().family(),
                            QApplication::font().pointSize());
    REQUIRE(QIcon::themeName() == "aide-dark");
}

TEST_CASE("AppearanceManager Dark theme uses light icon set",
          "[AppearanceManager]")
{
    int argc{1};
    // NOLINTNEXTLINE
    std::array<char*, 1> appName{{const_cast<char*>("aide_test")}};
    const QApplication app{argc, appName.data()};

    AppearanceManager manager{std::make_shared<MockSettings>()};
    manager.applyAppearance("Dark", QApplication::font().family(),
                            QApplication::font().pointSize());
    REQUIRE(QIcon::themeName() == "aide-light");
}

TEST_CASE(
    "AppearanceManager System theme icon set matches resolved color scheme",
    "[AppearanceManager]")
{
    int argc{1};
    // NOLINTNEXTLINE
    std::array<char*, 1> appName{{const_cast<char*>("aide_test")}};
    const QApplication app{argc, appName.data()};

    AppearanceManager manager{std::make_shared<MockSettings>()};
    manager.applyAppearance("System", QApplication::font().family(),
                            QApplication::font().pointSize());

    const auto scheme = manager.colorScheme();
    const auto* expected =
        (scheme == ColorScheme::Light) ? "aide-dark" : "aide-light";
    REQUIRE(QIcon::themeName() == expected);
}
