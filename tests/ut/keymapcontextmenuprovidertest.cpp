#include <array>

#include <catch2/catch.hpp>

#include <QApplication>

#include <settings/keymap/keymapcontextmenuprovider.hpp>

#include "mockkeymappagewidget.hpp"

using aide::core::KeymapContextMenuProvider;
using aide::test::MockKeyMapPageWidget;

TEST_CASE("Any keymap context menu provider")
{
    int numberOfArgs{1};
    // NOLINTNEXTLINE
    std::array<char*, 1> appName{{const_cast<char*>("aide_test")}};

    QApplication app{numberOfArgs, appName.data()};

    auto widget = std::make_shared<MockKeyMapPageWidget>();

    KeymapContextMenuProvider provider{widget.get()};

    SECTION("does not provide a menu if no action is assigned to tree item")
    {
        provider.createAndShowContextMenu();

        REQUIRE(widget->wasContextMeuSet());
        REQUIRE(widget->numberOfContextMenuEntries() == 0);
    }
}
