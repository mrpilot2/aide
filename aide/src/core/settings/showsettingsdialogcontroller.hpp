#ifndef AIDE_SHOW_SETTINGS_DIALOG_CONTROLLER_HPP
#define AIDE_SHOW_SETTINGS_DIALOG_CONTROLLER_HPP

class QItemSelection;

namespace aide
{
    class HierarchicalId;
} // namespace aide

namespace aide::core
{
    class ShowSettingsDialogController
    {
    public:
        virtual ~ShowSettingsDialogController() = default;

        virtual void showSettingsDialog() = 0;

        // Opens the dialog with a specific page pre-selected (identified by
        // SettingsPage::group()), instead of the usual last-selected/first
        // page fallback.
        virtual void showSettingsDialog(
            const aide::HierarchicalId& selectedGroup) = 0;
    };
} // namespace aide::core

#endif // AIDE_SHOW_SETTINGS_DIALOG_CONTROLLER_HPP
