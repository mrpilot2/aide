#ifndef AIDE_SHOW_SETTINGS_DIALOG_CONTROLLER_HPP
#define AIDE_SHOW_SETTINGS_DIALOG_CONTROLLER_HPP

class QItemSelection;

namespace aide::core
{
    class ShowSettingsDialogController
    {
    public:
        virtual ~ShowSettingsDialogController() = default;

        virtual void showSettingsDialog() = 0;
    };
} // namespace aide::core

#endif // AIDE_SHOW_SETTINGS_DIALOG_CONTROLLER_HPP
