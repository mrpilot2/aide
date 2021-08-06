#include <array>
#include <utility>

#include <catch2/catch.hpp>

#include <QAction>
#include <QApplication>
#include <QKeySequence>

#include "actionregistry.hpp"
#include "hierarchicalid.hpp"
#include "mocksettings.hpp"
#include "nulllogger.hpp"

using aide::ActionRegistry;
using aide::HierarchicalId;
using aide::test::MockSettings;
using aide::test::NullLogger;

TEST_CASE("A new action registry ")
{
    SECTION(" has no actions registered")
    {
        MockSettings settings;
        ActionRegistry registry{settings, std::make_shared<NullLogger>()};

        REQUIRE(registry.actions().empty());
    }
}

TEST_CASE("Any action registry ")
{
    MockSettings settings;
    ActionRegistry registry{settings, std::make_shared<NullLogger>()};

    int numberOfArgs{1};
    // NOLINTNEXTLINE
    std::array<char*, 1> appName{{const_cast<char*>("aide_test")}};

    QApplication::setApplicationName("aide_test");
    QApplication::setOrganizationName("aide_company");

    QApplication app{numberOfArgs, appName.data()};

    SECTION("can register an action with description and default key sequence")
    {
        auto action = std::make_shared<QAction>("&File", nullptr);
        const HierarchicalId id{HierarchicalId("MainMenu")("File")};
        QKeySequence defaultKeySequence(QKeySequence::Quit);
        const std::string description("Quit the application");

        registry.registerAction(action, id, description, {defaultKeySequence});

        REQUIRE(registry.actions().size() == 1);
    }

    SECTION("can register an action with description only")
    {
        auto action = std::make_shared<QAction>("&File", nullptr);
        const HierarchicalId id{HierarchicalId("MainMenu")("File")};
        const std::string description("Quit the application");

        registry.registerAction(action, id, description);

        REQUIRE(registry.actions().size() == 1);
        REQUIRE(action->shortcut().isEmpty());
        REQUIRE(action->statusTip().toStdString() == "Quit the application");
    }

    SECTION("can register an action with key sequence but without description")
    {
        auto action = std::make_shared<QAction>("&File", nullptr);
        const HierarchicalId id{HierarchicalId("MainMenu")("File")};
        QKeySequence defaultKeySequence(QKeySequence::Quit);

        registry.registerAction(action, id, {defaultKeySequence});

        REQUIRE(registry.actions().size() == 1);
        REQUIRE(action->shortcut() == QKeySequence(QKeySequence::Quit));
        REQUIRE(action->statusTip().isEmpty());
    }

    SECTION("can register an action without key sequence and description")
    {
        auto action = std::make_shared<QAction>("&File", nullptr);
        const HierarchicalId id{HierarchicalId("MainMenu")("File")};

        registry.registerAction(action, id);

        REQUIRE(registry.actions().size() == 1);
        REQUIRE(registry.actions().at(id).defaultKeySequences.isEmpty());
        REQUIRE(registry.actions().at(id).description.empty());
    }

    SECTION("does not allow duplicate actions")
    {
        auto action = std::make_shared<QAction>("&File", nullptr);
        const HierarchicalId id{HierarchicalId("MainMenu")("File")};

        registry.registerAction(action, id);
        registry.registerAction(action, id);

        REQUIRE(registry.actions().size() == 1);
    }

    SECTION("allows multiple key sequences")
    {
        auto action = std::make_shared<QAction>("&File", nullptr);
        const HierarchicalId id{HierarchicalId("MainMenu")("File")};

        registry.registerAction(
            action, id,
            std::vector<QKeySequence>(
                {QKeySequence("Alt+F4"), QKeySequence("Alt+F3")}));

        REQUIRE(registry.actions().at(id).defaultKeySequences.size() == 2);
        REQUIRE(action->shortcuts().size() == 2);
    }

    SECTION("overwrites key sequences from settings if present")
    {
        auto action = std::make_shared<QAction>("&File", nullptr);
        const HierarchicalId id{HierarchicalId("MainMenu")("File")};

        const auto settingsKey{HierarchicalId("Keymap")("MainMenu")("File")};

        settings.setValue(settingsKey, QKeySequence("Alt+F5"));

        registry.registerAction(
            action, id, std::vector<QKeySequence>({QKeySequence("Alt+F4")}));

        REQUIRE(action->shortcuts().size() == 1);
        REQUIRE(action->shortcut().toString().toStdString() ==
                QKeySequence("Alt+F5").toString().toStdString());
    }

    SECTION("allows to overwrite key sequences with empty key sequence")
    {
        auto action = std::make_shared<QAction>("&File", nullptr);
        const HierarchicalId id{HierarchicalId("MainMenu")("File")};

        const auto settingsKey{HierarchicalId("Keymap")("MainMenu")("File")};

        settings.setValue(settingsKey, QKeySequence());

        registry.registerAction(
            action, id, std::vector<QKeySequence>({QKeySequence("Alt+F4")}));

        REQUIRE(action->shortcuts().empty());
    }

    SECTION("allows to modify shortcuts for an action id")
    {
        auto action = std::make_shared<QAction>("&File", nullptr);
        const HierarchicalId id{HierarchicalId("MainMenu")("File")};

        const auto settingsKey{HierarchicalId("Keymap")("MainMenu")("File")};

        settings.setValue(settingsKey, QKeySequence("Alt+F5"));

        registry.registerAction(
            action, id, std::vector<QKeySequence>({QKeySequence("Alt+F4")}));

        registry.modifyShortcutsForAction(
            id, QList<QKeySequence>({QKeySequence("Alt+F6")}));

        REQUIRE(action->shortcuts() ==
                QList<QKeySequence>({QKeySequence("Alt+F6")}));
        REQUIRE(registry.actions().at(id).keySequences ==
                QList<QKeySequence>({QKeySequence("Alt+F6")}));
        REQUIRE(settings.value(settingsKey).toString().toStdString() ==
                "Alt+F6");
    }

    SECTION("uses default shortcuts if modified shortcuts equal default")
    {
        auto action = std::make_shared<QAction>("&File", nullptr);
        const HierarchicalId id{HierarchicalId("MainMenu")("File")};

        const auto settingsKey{HierarchicalId("Keymap")("MainMenu")("File")};

        settings.setValue(settingsKey, QKeySequence("Alt+F5"));

        registry.registerAction(
            action, id, std::vector<QKeySequence>({QKeySequence("Alt+F4")}));

        registry.modifyShortcutsForAction(
            id, QList<QKeySequence>({QKeySequence("Alt+F4")}));

        REQUIRE(action->shortcuts() ==
                QList<QKeySequence>({QKeySequence("Alt+F4")}));
        REQUIRE(registry.actions().at(id).keySequences.isEmpty());
        REQUIRE(settings.value(settingsKey) == QVariant());
    }
}
