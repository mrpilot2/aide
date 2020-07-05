#include <array>
#include <utility>

#include <catch2/catch.hpp>

#include <QAction>
#include <QApplication>
#include <QKeySequence>

#include "actionregistry.hpp"
#include "hierarchicalid.hpp"

using aide::ActionRegistry;
using aide::HierarchicalId;

TEST_CASE("A new action registry ")
{
    SECTION(" has no actions registered")
    {
        ActionRegistry registry;

        REQUIRE(registry.actions().empty());
    }
}

TEST_CASE("Any action registry ")
{
    ActionRegistry registry;

    int numberOfArgs{1};
    // NOLINTNEXTLINE
    std::array<char*, 1> appName{{const_cast<char*>("aide_test")}};

    QApplication app{numberOfArgs, appName.data()};

    SECTION("can register an action with description and default key sequence")
    {
        auto action = std::make_shared<QAction>("&File", nullptr);
        const HierarchicalId id{HierarchicalId("MainMenu")("File")};
        QKeySequence defaultKeySequence(QKeySequence::Quit);
        const std::string description("Quit the application");

        registry.registerAction(action, id, description, defaultKeySequence);

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

        registry.registerAction(action, id, defaultKeySequence);

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
        REQUIRE(registry.actions().at(id).defaultKeySequence.isEmpty());
        REQUIRE(registry.actions().at(id).description.empty());
    }
}