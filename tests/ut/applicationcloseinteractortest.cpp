#include <memory>

#include <catch2/catch.hpp>

#include "applicationclose.hpp"
#include "mockapplicationcloseview.hpp"

using aide::core::ApplicationClose;
using aide::test::MockApplicationCloseView;

TEST_CASE("Any application close interactor")
{
    auto appCloseView = std::make_shared<MockApplicationCloseView>();

    ApplicationClose appClose(appCloseView);

    SECTION("asks user to confirm close")
    {
        [[maybe_unused]] auto res = appClose.isCloseAllowed();
        REQUIRE(appCloseView->wasUserAsked());
    }
}
