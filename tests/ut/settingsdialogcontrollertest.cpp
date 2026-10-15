#include <array>

#include <catch2/catch_test_macros.hpp>

#include <QItemSelection>
#include <QString>

#include "settings/settingsdialogcontroller.hpp"
#include "spychangepagecontroller.hpp"

using aide::gui::SettingsDialogController;
using aide::test::SpyChangePageController;

TEST_CASE(
    "SettingsDialogController delegates to SettingsDialogChangePageController")
{
    SpyChangePageController spy;
    const SettingsDialogController controller(spy);

    SECTION("onUserChangedSelectedPage calls changeSelectedPage")
    {
        controller.onUserChangedSelectedPage(QItemSelection(),
                                             QItemSelection());

        REQUIRE(spy.changeSelectedPageCalled);
    }

    SECTION("onUserChangedAGuiElement calls anyGuiElementHasChanged")
    {
        controller.onUserChangedAGuiElement();

        REQUIRE(spy.anyGuiElementChangedCalled);
    }

    SECTION("onUserWantsToResetCurrentPage calls resetCurrentPage")
    {
        controller.onUserWantsToResetCurrentPage();

        REQUIRE(spy.resetCurrentPageCalled);
    }

    SECTION("onUserWantsToApplySettingsPages calls applyModifiedSettingsPages")
    {
        controller.onUserWantsToApplySettingsPages();

        REQUIRE(spy.applyModifiedPagesCalled);
    }

    SECTION("onUserChangedSearchPattern forwards the pattern")
    {
        controller.onUserChangedSearchPattern("needle");

        REQUIRE(spy.searchPatternChangedCalled);
        REQUIRE(spy.lastSearchPattern == "needle");
    }

    SECTION("onUserCommittedSearchPattern commits the current pattern")
    {
        controller.onUserCommittedSearchPattern();

        REQUIRE(spy.commitCurrentSearchPatternCalled);
    }

    SECTION("onUserWantsHelpForCurrentPage shows help for the current page")
    {
        controller.onUserWantsHelpForCurrentPage();

        REQUIRE(spy.showHelpForCurrentPageCalled);
    }
}
