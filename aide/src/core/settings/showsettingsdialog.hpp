#ifndef AIDE_SHOW_SETTINGS_DIALOG_HPP
#define AIDE_SHOW_SETTINGS_DIALOG_HPP

#include "loggerinterface.hpp"
#include "settingsdialogchangepagecontroller.hpp"
#include "settingsdialoginterface.hpp"
#include "settingspagegrouptreemodel.hpp"
#include "showsettingsdialogcontroller.hpp"

namespace aide::core
{
    class ShowSettingsDialog
        : public ShowSettingsDialogController
        , public SettingsDialogChangePageController
    {
    public:
        explicit ShowSettingsDialog(SettingsDialogWeakPtr dialog,
                                    LoggerPtr loggerInterface);

        void showSettingsDialog() override;

        void changeSelectedPage(const QItemSelection& selected,
                                const QItemSelection& deselected) override;

    private:
        void checkChangeSelectedPagePreConditions(
            const QItemSelection& selected) const;

        SettingsDialogWeakPtr settingsDialog;

        std::shared_ptr<SettingsPageGroupTreeModel> treeModel;
        const LoggerPtr logger;
        void updateDisplayName(const QModelIndex& selectedIndex) const;
    };
} // namespace aide::core

#endif // AIDE_SHOW_SETTINGS_DIALOG_HPP
