#include <array>

#include <catch2/catch_test_macros.hpp>

#include "commonsettingskeys.hpp"
#include "mocksettings.hpp"
#include "mocksettingsdialog.hpp"
#include "settings/geometryandstatedata.hpp"
#include "settings/settingsdialoggeometryandstate.hpp"

using aide::core::SettingsDialogGeometryAndState;
using aide::core::SettingsDialogGeometryAndStateData;
using aide::core::settings::KEYS;
using aide::test::MockSettings;
using aide::test::MockSettingsDialog;

TEST_CASE("Any settings dialog geometry and state use case")
{
    auto settingsDialog = std::make_shared<MockSettingsDialog>();
    auto settings       = MockSettings();

    SettingsDialogGeometryAndState useCase(settingsDialog, settings);

    SettingsDialogGeometryAndStateData dataToBeSaved{
        QByteArray{"abc"}, QByteArray{"def"}, QByteArray{"123"},
        QByteArray{"456"}, QString{"Group1/Group2"}};

    SECTION("saves dialog geometry in settings")
    {
        useCase.saveGeometryAndState(dataToBeSaved);

        REQUIRE(dataToBeSaved.dialogGeometry ==
                settings.value(KEYS().UI.SETTINGS_DIALOG_GEOMETRY_KEY)
                    .toByteArray());
    }

    SECTION("saves splitter geometry in settings")
    {
        useCase.saveGeometryAndState(dataToBeSaved);

        REQUIRE(dataToBeSaved.splitterGeometry ==
                settings.value(KEYS().UI.SETTINGS_DIALOG_SPLITTER_GEOMETRY_KEY)
                    .toByteArray());
    }

    SECTION("saves splitter state in settings")
    {
        useCase.saveGeometryAndState(dataToBeSaved);

        REQUIRE(dataToBeSaved.splitterState ==
                settings.value(KEYS().UI.SETTINGS_DIALOG_SPLITTER_STATE_KEY)
                    .toByteArray());
    }

    SECTION("saves tree view geometry in settings")
    {
        useCase.saveGeometryAndState(dataToBeSaved);

        REQUIRE(dataToBeSaved.treeViewGeometry ==
                settings.value(KEYS().UI.SETTINGS_DIALOG_TREE_VIEW_GEOMETRY_KEY)
                    .toByteArray());
    }

    SECTION("saves tree view selected item in settings")
    {
        useCase.saveGeometryAndState(dataToBeSaved);

        REQUIRE(
            dataToBeSaved.selectedTreeViewItem ==
            settings
                .value(KEYS().UI.SETTINGS_DIALOG_TREE_VIEW_SELECTED_ITEM_KEY)
                .toByteArray());
    }

    SECTION("restores geometry and state data")
    {
        settings.setValue(KEYS().UI.SETTINGS_DIALOG_GEOMETRY_KEY,
                          dataToBeSaved.dialogGeometry);
        settings.setValue(KEYS().UI.SETTINGS_DIALOG_SPLITTER_GEOMETRY_KEY,
                          dataToBeSaved.splitterGeometry);
        settings.setValue(KEYS().UI.SETTINGS_DIALOG_SPLITTER_STATE_KEY,
                          dataToBeSaved.splitterState);
        settings.setValue(KEYS().UI.SETTINGS_DIALOG_TREE_VIEW_GEOMETRY_KEY,
                          dataToBeSaved.treeViewGeometry);
        settings.setValue(KEYS().UI.SETTINGS_DIALOG_TREE_VIEW_SELECTED_ITEM_KEY,
                          dataToBeSaved.selectedTreeViewItem);

        useCase.restoreGeometryAndState();

        REQUIRE(settingsDialog->currentGeometry() == dataToBeSaved);
    }
}
