#include <memory>

#include <catch2/catch_test_macros.hpp>

#include <QCheckBox>
#include <QGroupBox>
#include <QLabel>
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
