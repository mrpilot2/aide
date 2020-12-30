#include "mocksettingsdialog.hpp"

using aide::test::MockSettingsDialog;

void MockSettingsDialog::executeDialog()
{
    settingsDialogWasExecuted = true;
}

bool MockSettingsDialog::wasSettingsDialogExecuted() const
{
    return settingsDialogWasExecuted;
}

void MockSettingsDialog::setTreeModel(
    std::shared_ptr<QAbstractItemModel> /*model*/)
{}
