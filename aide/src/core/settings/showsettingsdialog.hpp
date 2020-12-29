#ifndef AIDE_SHOW_SETTINGS_DIALOG_HPP
#define AIDE_SHOW_SETTINGS_DIALOG_HPP

#include "settingsdialoginterface.hpp"
#include "showsettingsdialogcontroller.hpp"

namespace aide::core
{
    class ShowSettingsDialog : public ShowSettingsDialogController
    {
    public:
        explicit ShowSettingsDialog(SettingsDialogWeakPtr dialog);

        void showSettingsDialog() const override;

    private:
        SettingsDialogWeakPtr settingsDialog;
    };
} // namespace aide::core

#endif // AIDE_SHOW_SETTINGS_DIALOG_HPP
