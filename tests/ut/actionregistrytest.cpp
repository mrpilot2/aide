#include <array>
#include <utility>

#include <catch2/catch.hpp>

#include <QAction>
#include <QApplication>
#include <QKeySequence>

#include "actionregistry.hpp"
#include "hierarchicalid.hpp"
#include "nulllogger.hpp"

using aide::ActionRegistry;
using aide::HierarchicalId;
using aide::test::NullLogger;

TEST_CASE("A new action registry ")
{
    SECTION(" has no actions registered")
    {
        ActionRegistry registry{std::make_shared<NullLogger>()};

        REQUIRE(registry.actions().empty());
    }
}

TEST_CASE("Any action registry ")
{
    ActionRegistry registry{std::make_shared<NullLogger>()};

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
    }
}
