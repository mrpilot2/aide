#include "catch2/catch.hpp"
#include "mocksettingsdialog.hpp"
#include "settings/showsettingsdialog.hpp"
#include "settings/showsettingsdialogcontroller.hpp"

using aide::core::ShowSettingsDialog;
using aide::core::ShowSettingsDialogController;
using aide::test::MockSettingsDialog;

TEST_CASE("Any show settings dialog use case")
{
    auto view = std::make_shared<MockSettingsDialog>();
    ShowSettingsDialog useCase{view};

    SECTION("shows settings dialog if requested by user")
    {
        useCase.showSettingsDialog();

        REQUIRE(view->wasSettingsDialogExecuted());
    }

    SECTION("works with base class pointer")
    {
        std::unique_ptr<ShowSettingsDialogController> base =
            std::make_unique<ShowSettingsDialog>(view);

        base->showSettingsDialog();

        REQUIRE(view->wasSettingsDialogExecuted());
    }
}
