#include "showsettingsdialog.hpp"

#include "settingspagegrouptreemodel.hpp"

using aide::core::SettingsPageGroupTreeModel;
using aide::core::ShowSettingsDialog;

aide::core::ShowSettingsDialog::ShowSettingsDialog(
    aide::core::SettingsDialogWeakPtr dialog)
    : settingsDialog{std::move(dialog)}
{}

void ShowSettingsDialog::showSettingsDialog() const
{
    auto dialog = settingsDialog.lock();

    if (dialog == nullptr) { return; }

    auto treeModel = std::make_shared<SettingsPageGroupTreeModel>();
    dialog->setTreeModel(treeModel);
    dialog->executeDialog();
}
