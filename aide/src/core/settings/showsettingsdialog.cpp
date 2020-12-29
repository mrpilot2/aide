#include "showsettingsdialog.hpp"

using aide::core::ShowSettingsDialog;

aide::core::ShowSettingsDialog::ShowSettingsDialog(
    aide::core::SettingsDialogWeakPtr dialog)
    : settingsDialog{std::move(dialog)}
{}

void ShowSettingsDialog::showSettingsDialog() const
{
    auto dialog = settingsDialog.lock();

    if (dialog == nullptr) { return; }

    dialog->executeDialog();
}
