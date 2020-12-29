#ifndef AIDE_SHOW_SETTINGS_DIALOG_CONTROLLER_HPP
#define AIDE_SHOW_SETTINGS_DIALOG_CONTROLLER_HPP

namespace aide::core
{
    class ShowSettingsDialogController
    {
    public:
        virtual ~ShowSettingsDialogController() = default;

        virtual void showSettingsDialog() const = 0;
    };
} // namespace aide::core

#endif // AIDE_SHOW_SETTINGS_DIALOG_CONTROLLER_HPP
