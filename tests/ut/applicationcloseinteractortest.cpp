#include <memory>

#include <catch2/catch.hpp>

#include "applicationclose.hpp"
#include "hierarchicalid.hpp"
#include "mockapplicationcloseview.hpp"
#include "mocksettings.hpp"

using aide::HierarchicalId;
using aide::core::ApplicationClose;
using aide::test::MockApplicationCloseView;
using aide::test::MockSettings;

TEST_CASE("Any application close interactor")
{
    auto appCloseView = std::make_shared<MockApplicationCloseView>();
    auto settings     = MockSettings();

    ApplicationClose appClose(appCloseView, settings);

    SECTION("asks user to confirm close if option is set")
    {
        [[maybe_unused]] auto res = appClose.isCloseAllowed();
        REQUIRE(appCloseView->wasUserAsked());
    }

    SECTION("does not ask user to confirm close if option is not set")
    {
        const auto askExitConfirmationKeyGroup =
            HierarchicalId("System")("Behavior");
        settings.setValue(askExitConfirmationKeyGroup, "AskExitConfirmation",
                          false);

        auto res = appClose.isCloseAllowed();
        REQUIRE(appCloseView->wasUserAsked() == false);
        REQUIRE(res == true);
    }
}
