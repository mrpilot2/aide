#include "settingsdialogcontroller.hpp"

using aide::gui::SettingsDialogController;

SettingsDialogController::SettingsDialogController(
    aide::core::SettingsDialogChangePageController& changePageController)
    : controller{changePageController}
{}

void SettingsDialogController::onUserChangedSelectedPage(
    const QItemSelection& selected, const QItemSelection& deselected)
{
    controller.changeSelectedPage(selected, deselected);
}

void SettingsDialogController::onUserChangedAGuiElement()
{
    controller.anyGuiElementHasChanged();
}
