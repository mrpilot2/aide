#include <memory>

#include <catch2/catch_test_macros.hpp>

#include <QCheckBox>
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
