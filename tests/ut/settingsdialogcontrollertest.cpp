#include <array>

#include <catch2/catch_test_macros.hpp>

#include <QItemSelection>
#include <QString>

#include "settings/settingsdialogchangepagecontroller.hpp"
#include "settings/settingsdialogcontroller.hpp"

using aide::core::SettingsDialogChangePageController;
using aide::gui::SettingsDialogController;

namespace
{
    class SpyController : public SettingsDialogChangePageController
    {
    public:
        bool changeSelectedPageCalled{false};
        bool anyGuiElementChangedCalled{false};
        bool resetCurrentPageCalled{false};
        bool applyModifiedPagesCalled{false};
        bool searchPatternChangedCalled{false};
        QString lastSearchPattern;

        void changeSelectedPage(const QItemSelection& /*selected*/,
                                const QItemSelection& /*deselected*/) override
        {
            changeSelectedPageCalled = true;
        }

        void searchPatternChanged(const QString& pattern) override
        {
            searchPatternChangedCalled = true;
            lastSearchPattern          = pattern;
        }

        void anyGuiElementHasChanged() override
        {
            anyGuiElementChangedCalled = true;
        }

        void resetCurrentPage() override { resetCurrentPageCalled = true; }

        void applyModifiedSettingsPages() override
        {
            applyModifiedPagesCalled = true;
        }
    };
} // namespace

TEST_CASE(
    "SettingsDialogController delegates to SettingsDialogChangePageController")
{
    SpyController spy;
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
}
