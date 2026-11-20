#include "settingsdialogcontroller.hpp"

#include <aide/helptopicid.hpp>

#include "contexthelptopic.hpp"

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

void SettingsDialogController::onUserChangedSearchPattern(
    const QString& pattern) const
{
    controller.searchPatternChanged(pattern);
}

void SettingsDialogController::onUserCommittedSearchPattern() const
{
    controller.commitCurrentSearchPattern();
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

void SettingsDialogController::onUserWantsHelpForCurrentPage() const
{
    controller.showHelpForCurrentPage();
}

void SettingsDialogController::onUserWantsContextHelp() const
{
    controller.showContextHelp(focusedContextHelpTopic());
}
