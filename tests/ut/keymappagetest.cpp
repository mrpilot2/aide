#include <array>

#include <catch2/catch.hpp>

#include <QApplication>

#include "actionregistry.hpp"
#include "mockkeymappagewidget.hpp"
#include "mocksettings.hpp"
#include "nulllogger.hpp"
#include "settings/keymap/keymappage.hpp"

using aide::ActionRegistry;
using aide::core::KeymapPage;
using aide::test::MockKeyMapPageWidget;
using aide::test::MockSettings;
using aide::test::NullLogger;

TEST_CASE("A new keymap page")
{
    int numberOfArgs{1};
    // NOLINTNEXTLINE
    std::array<char*, 1> appName{{const_cast<char*>("aide_test")}};

    QApplication app{numberOfArgs, appName.data()};

    MockSettings settings;
    auto logger = std::make_shared<NullLogger>();
    auto registry(std::make_shared<ActionRegistry>(settings, logger));

    auto widget = std::make_shared<MockKeyMapPageWidget>();

    KeymapPage page{registry, widget.get()};

    SECTION("returns a valid widget") { REQUIRE(page.widget() != nullptr); }

    SECTION("returns a valid keymap widget")
    {
        REQUIRE(page.keyMapWidget() != nullptr);
    }

    SECTION("is not modified by default")
    {
        REQUIRE(page.isModified() == false);
    }
}

TEST_CASE("Any keymap page")
{
    int numberOfArgs{1};
    // NOLINTNEXTLINE
    std::array<char*, 1> appName{{const_cast<char*>("aide_test")}};

    QApplication app{numberOfArgs, appName.data()};

    MockSettings settings;
    auto logger = std::make_shared<NullLogger>();
    auto registry(std::make_shared<ActionRegistry>(settings, logger));

    auto widget = std::make_unique<MockKeyMapPageWidget>();

    KeymapPage page{registry, widget.get()};

    SECTION("fills the tree model on reset")
    {
        page.reset();

        REQUIRE(widget->wasTreeModelSet());
    }
}
