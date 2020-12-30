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
