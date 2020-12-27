#include <memory>

#include <catch2/catch.hpp>

#include <QVariant>

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
            HierarchicalId("System")("Behavior")("AskExitConfirmation");
        settings.setValue(askExitConfirmationKeyGroup, false);

        auto res = appClose.isCloseAllowed();
        REQUIRE(appCloseView->wasUserAsked() == false);
        REQUIRE(res == true);
    }

    SECTION("allows to close application on user decision")
    {
        appCloseView->userShallClickExit();

        [[maybe_unused]] auto res = appClose.isCloseAllowed();
        REQUIRE(appCloseView->wasUserAsked());
        REQUIRE(res == true);
    }

    SECTION(" does not close application if user clicks Cancel")
    {
        appCloseView->userShallClickCancel();

        [[maybe_unused]] auto res = appClose.isCloseAllowed();
        REQUIRE(appCloseView->wasUserAsked());
        REQUIRE(res == false);
    }

    SECTION(" updates setting if user decides to not be asked again")
    {
        appCloseView->userShallSelectToNotBeAskedAgain();

        [[maybe_unused]] auto res = appClose.isCloseAllowed();

        const auto askExitConfirmationKeyGroup =
            HierarchicalId("System")("Behavior")("AskExitConfirmation");

        REQUIRE(settings.value(askExitConfirmationKeyGroup).toBool() == false);
    }

    SECTION(" does not change setting if user decides to be asked again")
    {
        const auto askExitConfirmationKeyGroup =
            HierarchicalId("System")("Behavior")("AskExitConfirmation");
        settings.setValue(askExitConfirmationKeyGroup, true);

        appCloseView->userShallSelectToBeAskedAgain();

        [[maybe_unused]] auto res = appClose.isCloseAllowed();

        REQUIRE(
            settings.value(askExitConfirmationKeyGroup, "AskExitConfirmation")
                .toBool() == true);
    }
}
