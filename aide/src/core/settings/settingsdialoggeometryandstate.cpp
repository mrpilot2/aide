#include "settingsdialoggeometryandstate.hpp"

#include <utility>

#include "commonsettingskeys.hpp"
#include "settingsinterface.hpp"

using aide::core::SettingsDialogGeometryAndState;
using aide::core::SettingsDialogWeakPtr;
using aide::core::settings::KEYS;

SettingsDialogGeometryAndState::SettingsDialogGeometryAndState(
    SettingsDialogWeakPtr v, SettingsInterface& unversionableSettings)
    : view{std::move(v)}
    , settings{unversionableSettings}
{}

void SettingsDialogGeometryAndState::saveGeometryAndState(
    const SettingsDialogGeometryAndStateData& data)
{
    settings.setValue(KEYS().UI.SETTINGS_DIALOG_GEOMETRY_KEY,
                      data.dialogGeometry);
    settings.setValue(KEYS().UI.SETTINGS_DIALOG_SPLITTER_GEOMETRY_KEY,
                      data.splitterGeometry);
    settings.setValue(KEYS().UI.SETTINGS_DIALOG_SPLITTER_STATE_KEY,
                      data.splitterState);
    settings.setValue(KEYS().UI.SETTINGS_DIALOG_TREE_VIEW_GEOMETRY_KEY,
                      data.treeViewGeometry);

    settings.setValue(KEYS().UI.SETTINGS_DIALOG_TREE_VIEW_SELECTED_ITEM_KEY,
                      data.selectedTreeViewItem);
}

void SettingsDialogGeometryAndState::restoreGeometryAndState()
{
    SettingsDialogGeometryAndStateData data;
    data.dialogGeometry =
        settings.value(KEYS().UI.SETTINGS_DIALOG_GEOMETRY_KEY).toByteArray();
    data.splitterGeometry =
        settings.value(KEYS().UI.SETTINGS_DIALOG_SPLITTER_GEOMETRY_KEY)
            .toByteArray();
    data.splitterState =
        settings.value(KEYS().UI.SETTINGS_DIALOG_SPLITTER_STATE_KEY)
            .toByteArray();
    data.treeViewGeometry =
        settings.value(KEYS().UI.SETTINGS_DIALOG_TREE_VIEW_GEOMETRY_KEY)
            .toByteArray();
    data.selectedTreeViewItem =
        settings.value(KEYS().UI.SETTINGS_DIALOG_TREE_VIEW_SELECTED_ITEM_KEY)
            .toString();
    {
        const auto ptr = view.lock();
        if (ptr == nullptr) { return; }

        ptr->restoreGeometryAndState(data);
    }
}

QString SettingsDialogGeometryAndState::selectedTreeViewItem() const
{
    return settings.value(KEYS().UI.SETTINGS_DIALOG_TREE_VIEW_SELECTED_ITEM_KEY)
        .toString();
}
