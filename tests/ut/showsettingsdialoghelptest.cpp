#include <memory>
#include <string>

#include <catch2/catch_test_macros.hpp>

#include <QAction>
#include <QItemSelection>
#include <QKeySequence>
#include <QList>
#include <QUrl>

#include <aide/aideconstants.hpp>
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
using aide::constants::CONSTANTS;
using aide::core::KeymapPage;
using aide::core::SettingsPageGroupTreeModel;
using aide::core::SettingsPageRegistry;
using aide::core::ShowHelpUseCase;
using aide::core::ShowSettingsDialog;
using aide::test::MockKeyMapPageWidget;
using aide::test::MockSettings;
using aide::test::MockSettingsDialog;
using aide::test::MockSettingsPage;
using aide::test::MockUrlLauncher;
using aide::test::NullLogger;

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
            : help{config, launcher, logger, registry}
        {}

        std::shared_ptr<MockSettingsDialog> view{
            std::make_shared<MockSettingsDialog>()};
        MockSettings settings;
        std::shared_ptr<NullLogger> logger{std::make_shared<NullLogger>()};
        std::shared_ptr<MockUrlLauncher> launcher{
            std::make_shared<MockUrlLauncher>()};
        SettingsPageRegistry registry;
        ShowHelpUseCase help;
        std::shared_ptr<ActionRegistry> actions{
            std::make_shared<ActionRegistry>(settings, logger)};
        ShowSettingsDialog useCase{view,   registry, settings,
                                   logger, help,     actions};

        // Registers the Context Help action the way the main window does
        // while help is active.
        void registerContextHelpAction(QAction& action) const
        {
            actions->registerAction(&action, CONSTANTS().HELP_CONTEXT_HELP,
                                    {QKeySequence(QKeySequence::HelpContents)});
        }

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

TEST_CASE("Settings dialog binds the selected page's topic to itself",
          "[ShowSettingsDialog][Help]")
{
    Fixture fixture;
    fixture.registry.addPage(pageWithTopic(HierarchicalId("A"), "keymap"));
    fixture.registry.addPage(
        std::make_shared<MockSettingsPage>(HierarchicalId("B")));
    const SettingsPageGroupTreeModel treeModel{fixture.registry};
    fixture.useCase.showSettingsDialog();

    fixture.select(treeModel.index(0, 0, QModelIndex()));
    REQUIRE(fixture.view->helpTopic() == HelpTopicId{"keymap"});

    fixture.select(treeModel.index(1, 0, QModelIndex()));
    REQUIRE(fixture.view->helpTopic().isEmpty());

    fixture.select(treeModel.index(0, 0, QModelIndex()));
    fixture.useCase.changeSelectedPage(QItemSelection(), QItemSelection());
    REQUIRE(fixture.view->helpTopic().isEmpty());
}

TEST_CASE("Settings dialog binds no topic for a group node",
          "[ShowSettingsDialog][Help]")
{
    Fixture fixture;
    fixture.registry.addPage(
        pageWithTopic(HierarchicalId("Group")("Leaf"), "keymap"));

    fixture.openAndSelectFirstRow();

    REQUIRE(fixture.view->helpTopic().isEmpty());
}

TEST_CASE("Settings dialog Context Help shortcut uses the current Keymap keys",
          "[ShowSettingsDialog][Help]")
{
    Fixture fixture;
    QAction contextHelp;
    fixture.registerContextHelpAction(contextHelp);

    SECTION("the default key while not remapped")
    {
        fixture.useCase.showSettingsDialog();

        REQUIRE(fixture.view->contextHelpShortcuts() ==
                QList<QKeySequence>{QKeySequence(QKeySequence::HelpContents)});
    }

    SECTION("the remapped key in a dialog opened after remapping")
    {
        const QKeySequence userShortcut(Qt::CTRL | Qt::SHIFT | Qt::Key_H);
        fixture.useCase.showSettingsDialog();

        fixture.actions->modifyShortcutsForAction(CONSTANTS().HELP_CONTEXT_HELP,
                                                  {userShortcut});
        fixture.useCase.showSettingsDialog();

        REQUIRE(fixture.view->contextHelpShortcuts() ==
                QList<QKeySequence>{userShortcut});
    }
}

TEST_CASE("Settings dialog has no Context Help shortcut while help is inactive",
          "[ShowSettingsDialog][Help]")
{
    SECTION("because the Context Help action is not registered")
    {
        Fixture fixture{ApplicationConfig{}};

        fixture.useCase.showSettingsDialog();

        REQUIRE(fixture.view->contextHelpShortcuts().isEmpty());
    }

    SECTION("because the use case knows no action registry")
    {
        MockSettings settings;
        const auto view = std::make_shared<MockSettingsDialog>();
        SettingsPageRegistry registry;
        ShowSettingsDialog useCase{view, registry, settings,
                                   std::make_shared<NullLogger>()};

        useCase.showSettingsDialog();

        REQUIRE(view->contextHelpShortcuts().isEmpty());
    }
}

TEST_CASE("Context Help in the settings dialog opens the resolved topic",
          "[ShowSettingsDialog][Help]")
{
    Fixture fixture;

    SECTION("when the topic is available")
    {
        fixture.useCase.showContextHelp(HelpTopicId{"keymap"});

        REQUIRE(fixture.launcher->lastRequestedUrl ==
                std::string("https://x/help/keymap/"));
    }

    SECTION("and falls back to Help Contents without a topic")
    {
        fixture.useCase.showContextHelp(HelpTopicId{});

        REQUIRE(fixture.launcher->lastRequestedUrl ==
                std::string("https://x/help/"));
    }
}

TEST_CASE("Context Help in the settings dialog launches nothing without help",
          "[ShowSettingsDialog][Help]")
{
    Fixture fixture{ApplicationConfig{}};

    fixture.useCase.showContextHelp(HelpTopicId{"keymap"});

    REQUIRE_FALSE(fixture.launcher->lastRequestedUrl.has_value());
}
