#include <memory>

#include <catch2/catch_test_macros.hpp>

#include "settings/settingsdialog.hpp"

using aide::gui::SettingsDialog;

TEST_CASE("SettingsDialog does not free a page it does not own",
          "[SettingsDialog]")
{
    // Settings pages are owned by the SettingsPageRegistry via shared_ptr,
    // typically a make_shared allocation where the widget lives inside the
    // control block. Showing a page hands it to the scroll area, which takes
    // Qt ownership. When the dialog is destroyed it must release the current
    // page instead of letting Qt delete/free a widget it does not own -
    // otherwise the make_shared block is freed twice (heap corruption / abort).
    auto page = std::make_shared<QWidget>();

    {
        SettingsDialog dialog;
        dialog.showSelectedPageWidget(page.get());
    }

    // The shared_ptr must still be the sole, valid owner after the dialog dies.
    REQUIRE(page.use_count() == 1);
    REQUIRE(page->parent() == nullptr);
}
