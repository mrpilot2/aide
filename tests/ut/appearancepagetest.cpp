#include <array>
#include <memory>

#include <catch2/catch_test_macros.hpp>

#include <QApplication>
#include <QComboBox>
#include <QFontComboBox>
#include <QPalette>
#include <QSpinBox>

#include <aide/appearancemanager.hpp>

#include "mocksettings.hpp"
#include "settings/appearancepage.hpp"

using aide::AppearanceManager;
using aide::gui::AppearancePage;
using aide::test::MockSettings;

namespace
{
    constexpr int TEST_FONT_SIZE   = 12;
    constexpr int OTHER_FONT_SIZE  = 16;
    constexpr int FONT_SIZE_OFFSET = 1;
} // namespace

TEST_CASE("AppearancePage initial state", "[AppearancePage]")
{
    AppearanceManager manager{std::make_shared<MockSettings>()};
    manager.applyAppearance("Light", QApplication::font().family(),
                            TEST_FONT_SIZE);

    AppearancePage page{manager};

    SECTION("is not modified")
    {
        REQUIRE_FALSE(page.isModified());
    }

    SECTION("shows active theme")
    {
        REQUIRE(page.widget() != nullptr);
        const auto* combo = page.widget()->findChild<QComboBox*>();
        REQUIRE(combo != nullptr);
        REQUIRE(combo->currentText() == "Light");
    }

    SECTION("shows active font size")
    {
        const auto* spinBox = page.widget()->findChild<QSpinBox*>();
        REQUIRE(spinBox != nullptr);
        REQUIRE(spinBox->value() == TEST_FONT_SIZE);
    }
}

TEST_CASE("AppearancePage modified detection", "[AppearancePage]")
{
    AppearanceManager manager{std::make_shared<MockSettings>()};
    manager.applyAppearance("Light", QApplication::font().family(),
                            TEST_FONT_SIZE);

    AppearancePage page{manager};

    SECTION("theme change marks page modified")
    {
        auto* combo = page.widget()->findChild<QComboBox*>();
        REQUIRE(combo != nullptr);
        combo->setCurrentText("Dark");
        REQUIRE(page.isModified());
    }

    SECTION("font size change marks page modified")
    {
        auto* spinBox = page.widget()->findChild<QSpinBox*>();
        REQUIRE(spinBox != nullptr);
        spinBox->setValue(OTHER_FONT_SIZE);
        REQUIRE(page.isModified());
    }
}

TEST_CASE("AppearancePage reset", "[AppearancePage]")
{
    AppearanceManager manager{std::make_shared<MockSettings>()};
    manager.applyAppearance("Light", QApplication::font().family(),
                            TEST_FONT_SIZE);

    AppearancePage page{manager};

    SECTION("restores controls to last-applied values")
    {
        auto* combo   = page.widget()->findChild<QComboBox*>();
        auto* spinBox = page.widget()->findChild<QSpinBox*>();
        REQUIRE(combo != nullptr);
        REQUIRE(spinBox != nullptr);

        combo->setCurrentText("Dark");
        spinBox->setValue(OTHER_FONT_SIZE);
        REQUIRE(page.isModified());

        page.reset();

        REQUIRE_FALSE(page.isModified());
        REQUIRE(combo->currentText() == "Light");
        REQUIRE(spinBox->value() == TEST_FONT_SIZE);
    }
}

TEST_CASE("AppearancePage picks up themes registered after construction",
          "[AppearancePage]")
{
    AppearanceManager manager{std::make_shared<MockSettings>()};
    manager.applyAppearance("Light", QApplication::font().family(),
                            TEST_FONT_SIZE);

    AppearancePage page{manager};

    // A consumer registers a theme after ApplicationBuilder (and thus the
    // page) has already been constructed.
    manager.registerTheme({.name            = "Demo Blue",
                           .palette         = QPalette{},
                           .iconThemeName   = "",
                           .iconSearchPaths = {}});

    auto* combo = page.widget()->findChild<QComboBox*>();
    REQUIRE(combo != nullptr);

    SECTION("theme is absent until the controls are refreshed")
    {
        REQUIRE(combo->findText("Demo Blue") == -1);
    }

    SECTION("reset refreshes the theme list")
    {
        page.reset();
        REQUIRE(combo->findText("Demo Blue") != -1);
    }

    SECTION("refreshed list keeps the active theme selected")
    {
        page.reset();
        REQUIRE(combo->currentText() == "Light");
        REQUIRE_FALSE(page.isModified());
    }
}

TEST_CASE("AppearancePage apply", "[AppearancePage]")
{
    AppearanceManager manager{std::make_shared<MockSettings>()};
    manager.applyAppearance("Light", QApplication::font().family(),
                            TEST_FONT_SIZE);

    AppearancePage page{manager};

    SECTION("applies changed appearance to manager")
    {
        auto* combo   = page.widget()->findChild<QComboBox*>();
        auto* spinBox = page.widget()->findChild<QSpinBox*>();
        REQUIRE(combo != nullptr);
        REQUIRE(spinBox != nullptr);

        combo->setCurrentText("Dark");
        const int newSize = TEST_FONT_SIZE + FONT_SIZE_OFFSET;
        spinBox->setValue(newSize);

        page.apply();

        REQUIRE_FALSE(page.isModified());
        REQUIRE(manager.activeThemeName() == "Dark");
        REQUIRE(manager.activeFont().pointSize() == newSize);
    }

    SECTION("marks page unmodified after apply")
    {
        auto* combo = page.widget()->findChild<QComboBox*>();
        REQUIRE(combo != nullptr);
        combo->setCurrentText("Dark");
        REQUIRE(page.isModified());

        page.apply();

        REQUIRE_FALSE(page.isModified());
    }
}
