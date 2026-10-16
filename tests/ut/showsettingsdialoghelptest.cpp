#include <memory>
#include <string>

#include <catch2/catch_test_macros.hpp>

#include <QItemSelection>
#include <QUrl>

#include <aide/applicationconfig.hpp>
#include <aide/helptopicid.hpp>

#include "actionregistry.hpp"
#include "hierarchicalid.hpp"
#include "mockkeymappagewidget.hpp"
#include "mocksettings.hpp"
#include "mocksettingsdialog.hpp"
#include "mocksettingspage.hpp"
#include "mockurllauncher.hpp"
#include "nulllogger.hpp"
#include "settings/keymap/keymappage.hpp"
#include "settings/settingspagegrouptreemodel.hpp"
#include "settings/settingspageregistry.hpp"
#include "settings/showsettingsdialog.hpp"
#include "showhelpusecase.hpp"

using aide::ActionRegistry;
using aide::ApplicationConfig;
using aide::HelpTopicId;
using aide::HierarchicalId;
using aide::core::KeymapPage;
using aide::core::SettingsPageGroupTreeModel;
using aide::core::SettingsPageRegistry;
using aide::core::ShowHelpUseCase;
using aide::core::ShowSettingsDialog;
using aide::test::MockKeyMapPageWidget;
using aide::test::MockSettings;
using aide::test::MockSettingsDialog;
using aide::test::MockSettingsPage;
using aide::test::NullLogger;
using aide::tests::MockUrlLauncher;

namespace
{
    ApplicationConfig activeHelpConfig()
    {
        ApplicationConfig config;
        config.setHelpBaseUrl(QUrl("https://x/help"));
        return config;
    }

    std::shared_ptr<MockSettingsPage> pageWithTopic(const HierarchicalId& group,
                                                    const char* topic)
    {
        auto page = std::make_shared<MockSettingsPage>(group);
        page->setHelpTopicId(HelpTopicId{topic});
        return page;
    }

    struct Fixture
    {
        explicit Fixture(const ApplicationConfig& config = activeHelpConfig())
            : help{config, launcher, logger}
        {}

        std::shared_ptr<MockSettingsDialog> view{
            std::make_shared<MockSettingsDialog>()};
        MockSettings settings;
        std::shared_ptr<NullLogger> logger{std::make_shared<NullLogger>()};
        std::shared_ptr<MockUrlLauncher> launcher{
            std::make_shared<MockUrlLauncher>()};
        SettingsPageRegistry registry;
        ShowHelpUseCase help;
        ShowSettingsDialog useCase{view, registry, settings, logger, help};

        void select(const QModelIndex& index)
        {
            useCase.changeSelectedPage({index, index}, QItemSelection());
        }

        // Shows the dialog and selects the top-level row of the tree.
        void openAndSelectFirstRow()
        {
            const SettingsPageGroupTreeModel treeModel{registry};
            useCase.showSettingsDialog();
            select(treeModel.index(0, 0, QModelIndex()));
        }
    };
} // namespace

TEST_CASE(
    "Settings dialog Help button visibility follows whether help is "
    "active",
    "[ShowSettingsDialog][Help]")
{
    SECTION("hidden when the feature is on but no Help Base URL is set")
    {
        Fixture fixture{ApplicationConfig{}};

        fixture.useCase.showSettingsDialog();

        REQUIRE_FALSE(fixture.view->isHelpButtonVisible());
    }

    SECTION("hidden when the feature is off even though a URL is set")
    {
        auto config = activeHelpConfig();
        config.setEnabled(ApplicationConfig::Feature::Help, false);
        Fixture fixture{config};

        fixture.useCase.showSettingsDialog();

        REQUIRE_FALSE(fixture.view->isHelpButtonVisible());
    }

    SECTION("shown when help is active")
    {
        Fixture fixture;

        fixture.useCase.showSettingsDialog();

        REQUIRE(fixture.view->isHelpButtonVisible());
    }
}

TEST_CASE("Settings dialog Help button opens the Keymap page's help topic",
          "[ShowSettingsDialog][Help]")
{
    // Declared first: the page keeps a raw pointer to it.
    MockKeyMapPageWidget keymapWidget;
    Fixture fixture;
    fixture.registry.addPage(std::make_shared<KeymapPage>(
        std::make_shared<ActionRegistry>(fixture.settings, fixture.logger),
        &keymapWidget));

    fixture.openAndSelectFirstRow();

    REQUIRE(fixture.view->isHelpButtonEnabled());

    fixture.useCase.showHelpForCurrentPage();

    REQUIRE(fixture.launcher->lastRequestedUrl ==
            std::string("https://x/help/keymap/"));
}

TEST_CASE("Settings dialog Help button is disabled without an available topic",
          "[ShowSettingsDialog][Help]")
{
    SECTION("for a page that declares no topic")
    {
        Fixture fixture;
        fixture.registry.addPage(
            std::make_shared<MockSettingsPage>(HierarchicalId("Plain")));

        fixture.openAndSelectFirstRow();

        REQUIRE_FALSE(fixture.view->isHelpButtonEnabled());
    }

    SECTION("for a group node without a page")
    {
        Fixture fixture;
        fixture.registry.addPage(
            pageWithTopic(HierarchicalId("Group")("Leaf"), "keymap"));

        fixture.openAndSelectFirstRow();

        REQUIRE_FALSE(fixture.view->isHelpButtonEnabled());
    }

    SECTION("for a page whose topic the client disabled")
    {
        auto config = activeHelpConfig();
        config.disableHelpTopic(HelpTopicId{"keymap"});
        Fixture fixture{config};
        fixture.registry.addPage(
            pageWithTopic(HierarchicalId("Keymap"), "keymap"));

        fixture.openAndSelectFirstRow();

        REQUIRE_FALSE(fixture.view->isHelpButtonEnabled());
    }

    SECTION("and clicking it launches nothing")
    {
        Fixture fixture;
        fixture.registry.addPage(
            std::make_shared<MockSettingsPage>(HierarchicalId("Plain")));

        fixture.openAndSelectFirstRow();
        fixture.useCase.showHelpForCurrentPage();

        REQUIRE_FALSE(fixture.launcher->lastRequestedUrl.has_value());
    }
}

TEST_CASE("Settings dialog Help button state updates on every selection change",
          "[ShowSettingsDialog][Help]")
{
    Fixture fixture;
    fixture.registry.addPage(pageWithTopic(HierarchicalId("A"), "keymap"));
    fixture.registry.addPage(
        std::make_shared<MockSettingsPage>(HierarchicalId("B")));
    const SettingsPageGroupTreeModel treeModel{fixture.registry};
    fixture.useCase.showSettingsDialog();

    fixture.select(treeModel.index(0, 0, QModelIndex()));
    REQUIRE(fixture.view->isHelpButtonEnabled());

    fixture.select(treeModel.index(1, 0, QModelIndex()));
    REQUIRE_FALSE(fixture.view->isHelpButtonEnabled());

    fixture.useCase.changeSelectedPage(QItemSelection(), QItemSelection());
    REQUIRE_FALSE(fixture.view->isHelpButtonEnabled());
}
