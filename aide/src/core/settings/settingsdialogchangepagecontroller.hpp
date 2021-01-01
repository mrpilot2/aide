#ifndef AIDE_SETTINGS_DIALOG_CHANGE_PAGE_CONTROLLER_HPP
#define AIDE_SETTINGS_DIALOG_CHANGE_PAGE_CONTROLLER_HPP

class QItemSelection;

namespace aide::core
{
    class SettingsDialogChangePageController
    {
    public:
        SettingsDialogChangePageController() = default;
        virtual ~SettingsDialogChangePageController();
        SettingsDialogChangePageController(
            const SettingsDialogChangePageController&) = delete;
        SettingsDialogChangePageController& operator   =(
            const SettingsDialogChangePageController&) = delete;
        SettingsDialogChangePageController(
            SettingsDialogChangePageController&&)   = delete;
        SettingsDialogChangePageController& operator=(
            SettingsDialogChangePageController&&) = delete;

        virtual void changeSelectedPage(const QItemSelection& selected,
                                        const QItemSelection& deselected) = 0;

        virtual void anyGuiElementHasChanged() = 0;
    };
} // namespace aide::core

#endif // AIDE_SETTINGS_DIALOG_CHANGE_PAGE_CONTROLLER_HPP
