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

void MockSettingsDialog::setSelectedPageDisplayName(
    const std::string& displayName)
{
    currentDisplayName = displayName;
}

const std::string& MockSettingsDialog::displayName() const
{
    return currentDisplayName;
}

void MockSettingsDialog::showSelectedPageWidget(QWidget* widget)
{
    settingsWidget = widget;
}

void MockSettingsDialog::showEmptyPageWidget() {}

QWidget* MockSettingsDialog::currentlyShownWidget() const
{
    return settingsWidget;
}
