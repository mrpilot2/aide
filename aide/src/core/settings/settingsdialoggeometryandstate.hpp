#ifndef AIDE_SETTINGS_DIALOG_GEOMETRY_AND_STATE_HPP
#define AIDE_SETTINGS_DIALOG_GEOMETRY_AND_STATE_HPP

#include "settingsdialoggeometryandstatecontroller.hpp"
#include "settingsdialoginterface.hpp"

namespace aide
{
    class SettingsInterface;
} // namespace aide

namespace aide::core
{
    class SettingsDialogGeometryAndState
        : public SettingsDialogGeometryAndStateController
    {
    public:
        SettingsDialogGeometryAndState(
            SettingsDialogWeakPtr v, SettingsInterface& unversionableSettings);

        void saveGeometryAndState(
            const SettingsDialogGeometryAndStateData& data) override;

        void restoreGeometryAndState() override;

        [[nodiscard]] QString selectedTreeViewItem() const;

    private:
        SettingsDialogWeakPtr view;
        SettingsInterface& settings;
    };
} // namespace aide::core

#endif // AIDE_SETTINGS_DIALOG_GEOMETRY_AND_STATE_HPP
