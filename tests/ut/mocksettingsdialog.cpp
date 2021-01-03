#include "mocksettingsdialog.hpp"

#include <iostream>

using aide::core::UserSelection;
using aide::test::MockSettingsDialog;

UserSelection MockSettingsDialog::executeDialog()
{
    settingsDialogWasExecuted = true;
    return userShallAcceptDialog ? UserSelection::Ok : UserSelection::Cancel;
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

void MockSettingsDialog::showResetLabel(bool show)
{
    resetLabelIsVisible = show;
}

bool MockSettingsDialog::isResetLabelVisible() const
{
    return resetLabelIsVisible;
}

void MockSettingsDialog::enableApplyButton(bool enable)
{
    applyButtonIsEnabled = enable;
}

bool MockSettingsDialog::isApplyButtonEnabled() const
{
    return applyButtonIsEnabled;
}

void MockSettingsDialog::simulateUserAcceptsDialog(bool accept)
{
    userShallAcceptDialog = accept;
}
void aide::test::MockSettingsDialog::restoreGeometryAndState(
    aide::core::SettingsDialogGeometryAndStateData data)
{
    geometryAndStateData = data;
}

void aide::test::MockSettingsDialog::setSelectedGroupIndex(
    const QModelIndex& index)
{
    selectedGroupIndex = index;
}

const QModelIndex& aide::test::MockSettingsDialog::getSelectedGroupIndex() const
{
    return selectedGroupIndex;
}

aide::core::SettingsDialogGeometryAndStateData
aide::test::MockSettingsDialog::currentGeometry() const
{
    return geometryAndStateData;
}
