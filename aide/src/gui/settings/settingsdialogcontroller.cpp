#include "settingsdialogcontroller.hpp"

using aide::gui::SettingsDialogController;

SettingsDialogController::SettingsDialogController(
    core::SettingsDialogChangePageController& changePageController)
    : controller{changePageController}
{}

void SettingsDialogController::onUserChangedSelectedPage(
    const QItemSelection& selected, const QItemSelection& deselected) const
{
    controller.changeSelectedPage(selected, deselected);
}

void SettingsDialogController::onUserChangedAGuiElement() const
{
    controller.anyGuiElementHasChanged();
}

void SettingsDialogController::onUserWantsToResetCurrentPage() const
{
    controller.resetCurrentPage();
}

void SettingsDialogController::onUserWantsToApplySettingsPages() const
{
    controller.applyModifiedSettingsPages();
}
