#include <memory>

#include <catch2/catch_test_macros.hpp>

#include <QCheckBox>
#include <QComboBox>
#include <QGraphicsOpacityEffect>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSlider>
#include <QSpinBox>
#include <QWidget>

#include <aide/hierarchicalid.hpp>
#include <aide/settings/settingspage.hpp>

#include "mocksettingspage.hpp"

using aide::HierarchicalId;
using aide::core::SettingsPage;
using aide::test::MockSettingsPage;

namespace
{
    class ContentSettingsPage : public SettingsPage
    {
    public:
        ContentSettingsPage()
            : SettingsPage(HierarchicalId("Test")("Content"))
            , pageWidget{std::make_unique<QWidget>()}
        {}

        QWidget* widget() override { return pageWidget.get(); }

        [[nodiscard]] bool isModified() const override { return false; }

        void reset() override {}

        void apply() override {}

    private:
        std::unique_ptr<QWidget> pageWidget;
    };
} // namespace

TEST_CASE("Any settings page")
{
    SECTION("stores the group it belongs to")
    {
        const auto expectedId{HierarchicalId("Test")("Mock")};

        const MockSettingsPage page;

        REQUIRE(expectedId == page.group());
    }
}

TEST_CASE("Searching a settings page")
{
    ContentSettingsPage page;

    SECTION("matches text contained in a child label")
    {
        auto* label = new QLabel("Enable dark mode", page.widget());
        (void)label;

        REQUIRE(page.matches("dark"));
    }

    SECTION("matches case-insensitively")
    {
        auto* label = new QLabel("Enable dark mode", page.widget());
        (void)label;

        REQUIRE(page.matches("DARK"));
    }

    SECTION("matches text contained in a child check box")
    {
        auto* checkBox = new QCheckBox("Show line numbers", page.widget());
        (void)checkBox;

        REQUIRE(page.matches("line numbers"));
    }

    SECTION("does not match when no child contains the pattern")
    {
        auto* label = new QLabel("Enable dark mode", page.widget());
        (void)label;

        REQUIRE_FALSE(page.matches("nonexistent"));
    }

    SECTION("matches when the words are spread across different widgets")
    {
        auto* first  = new QLabel("Font family", page.widget());
        auto* second = new QCheckBox("Show line numbers", page.widget());
        (void)first;
        (void)second;

        REQUIRE(page.matches("font numbers"));
    }

    SECTION("matches when only one of several words is present")
    {
        auto* label = new QLabel("Enable dark mode", page.widget());
        (void)label;

        REQUIRE(page.matches("dark nonexistent"));
    }

    SECTION("does not match when none of the words are present")
    {
        auto* label = new QLabel("Enable dark mode", page.widget());
        (void)label;

        REQUIRE_FALSE(page.matches("foo bar"));
    }
}

TEST_CASE("Highlighting a settings page")
{
    ContentSettingsPage page;

    SECTION("gives a matching label a brownish-orange border")
    {
        auto* label = new QLabel("Enable dark mode", page.widget());

        page.highlight("dark");

        REQUIRE(label->styleSheet().contains("#8B4513"));
    }

    SECTION("does not highlight a label that does not match")
    {
        auto* matching    = new QLabel("Enable dark mode", page.widget());
        auto* nonMatching = new QLabel("Show line numbers", page.widget());

        page.highlight("dark");

        REQUIRE(matching->styleSheet().contains("#8B4513"));
        REQUIRE_FALSE(nonMatching->styleSheet().contains("#8B4513"));
    }

    SECTION("gives a matching check box a brownish-orange border")
    {
        auto* checkBox = new QCheckBox("Show line numbers", page.widget());

        page.highlight("line numbers");

        REQUIRE(checkBox->styleSheet().contains("#8B4513"));
    }

    SECTION("updates highlights when the search pattern changes")
    {
        auto* label    = new QLabel("Enable dark mode", page.widget());
        auto* checkBox = new QCheckBox("Show line numbers", page.widget());

        page.highlight("dark");
        page.highlight("line numbers");

        REQUIRE_FALSE(label->styleSheet().contains("#8B4513"));
        REQUIRE(checkBox->styleSheet().contains("#8B4513"));
    }

    SECTION("removes all highlights when the search is cleared")
    {
        auto* label = new QLabel("Enable dark mode", page.widget());

        page.highlight("dark");
        page.highlight("");

        REQUIRE_FALSE(label->styleSheet().contains("#8B4513"));
    }

    SECTION("gives a matching group box a brownish-orange border")
    {
        auto* groupBox = new QGroupBox("Editor settings", page.widget());

        page.highlight("editor");

        REQUIRE(groupBox->styleSheet().contains("#8B4513"));
    }
}

namespace
{
    [[nodiscard]] bool isDimmed(const QWidget* widget)
    {
        const auto* effect = qobject_cast<const QGraphicsOpacityEffect*>(
            widget->graphicsEffect());
        return effect != nullptr && effect->opacity() < 1.0;
    }
} // namespace

TEST_CASE("Graying out non-matching content on a settings page")
{
    ContentSettingsPage page;

    SECTION("dims a non-matching label while a search is active")
    {
        auto* nonMatching = new QLabel("Show line numbers", page.widget());

        page.highlight("dark");

        REQUIRE(isDimmed(nonMatching));
    }

    SECTION("keeps a matching label at full opacity with its border")
    {
        auto* matching = new QLabel("Enable dark mode", page.widget());

        page.highlight("dark");

        REQUIRE_FALSE(isDimmed(matching));
        REQUIRE(matching->styleSheet().contains("#8B4513"));
    }

    SECTION("dims non-matching buttons and group boxes too")
    {
        auto* button   = new QPushButton("Show line numbers", page.widget());
        auto* groupBox = new QGroupBox("Editor settings", page.widget());

        page.highlight("dark");

        REQUIRE(isDimmed(button));
        REQUIRE(isDimmed(groupBox));
    }

    SECTION("removes the dimming from all widgets when the search is cleared")
    {
        auto* nonMatching = new QLabel("Show line numbers", page.widget());

        page.highlight("dark");
        page.highlight("");

        REQUIRE_FALSE(isDimmed(nonMatching));
    }

    SECTION("re-evaluates dimming when the search pattern changes")
    {
        auto* label = new QLabel("Enable dark mode", page.widget());

        page.highlight("dark");
        page.highlight("line");

        REQUIRE(isDimmed(label));
    }

    SECTION("dims non-text input controls while a search is active")
    {
        auto* comboBox = new QComboBox(page.widget());
        auto* lineEdit = new QLineEdit(page.widget());
        auto* spinBox  = new QSpinBox(page.widget());
        auto* slider   = new QSlider(page.widget());

        page.highlight("dark");

        REQUIRE(isDimmed(comboBox));
        REQUIRE(isDimmed(lineEdit));
        REQUIRE(isDimmed(spinBox));
        REQUIRE(isDimmed(slider));
    }

    SECTION("removes dimming from input controls when the search is cleared")
    {
        auto* comboBox = new QComboBox(page.widget());
        auto* spinBox  = new QSpinBox(page.widget());

        page.highlight("dark");
        page.highlight("");

        REQUIRE_FALSE(isDimmed(comboBox));
        REQUIRE_FALSE(isDimmed(spinBox));
    }

    SECTION("does not doubly dim the editor embedded in a spin box")
    {
        auto* spinBox = new QSpinBox(page.widget());

        page.highlight("dark");

        auto* embedded = spinBox->findChild<QLineEdit*>();
        REQUIRE(embedded != nullptr);
        REQUIRE_FALSE(isDimmed(embedded));
    }
}
