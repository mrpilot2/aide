#include <catch2/catch.hpp>

#include "actionregistry.hpp"
#include "hierarchicalid.hpp"
#include "mockkeymappagewidget.hpp"
#include "nulllogger.hpp"
#include "settings/keymap/showkeymap.hpp"

using aide::ActionRegistry;
using aide::HierarchicalId;
using aide::core::ShowKeyMap;
using aide::test::MockKeyMapPageWidget;
using aide::test::NullLogger;

TEST_CASE("Any show keymap use case")
{
    auto logger = std::make_shared<NullLogger>();
    auto registry(std::make_shared<ActionRegistry>(logger));

    auto widget = std::make_unique<MockKeyMapPageWidget>();
    ShowKeyMap keyMapUseCase{registry, widget.get()};

    SECTION("does not crash if widget is nullptr")
    {
        ShowKeyMap keyMapLocalUseCase{registry, nullptr};

        REQUIRE_NOTHROW(keyMapLocalUseCase.fillTreeView());
    }

    SECTION("sets tree model if widget is not null")
    {
        keyMapUseCase.fillTreeView();

        REQUIRE(widget->wasTreeModelSet());
    }

    SECTION("allows to set empty tree model")
    {
        keyMapUseCase.fillTreeView();

        REQUIRE(widget->numberOfRowsInTree() == 0);
    }

    SECTION("allows to set non empty tree model")
    {
        auto action = std::make_shared<QAction>("test");
        registry->registerAction(action, HierarchicalId("Menu"));

        keyMapUseCase.fillTreeView();
        REQUIRE(widget->numberOfRowsInTree() == 1);
    }
}
