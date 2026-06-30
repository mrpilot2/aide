#ifndef AIDE_SHOW_SETTINGS_DIALOG_HPP
#define AIDE_SHOW_SETTINGS_DIALOG_HPP

#include <QString>

#include <settings/settingspage.hpp>
#include <settings/settingspageregistry.hpp>

#include "loggerinterface.hpp"
#include "settingsdialogchangepagecontroller.hpp"
#include "settingsdialoggeometryandstate.hpp"
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
                                    SettingsPageRegistry& registry,
                                    SettingsInterface& settings,
                                    LoggerPtr loggerInterface);

        void showSettingsDialog() override;

        void changeSelectedPage(const QItemSelection& selected,
                                const QItemSelection& deselected) override;

        void searchPatternChanged(const QString& pattern) override;

        [[nodiscard]] const QString& currentSearchPattern() const;

        void anyGuiElementHasChanged() override;

        void resetCurrentPage() override;

        void applyModifiedSettingsPages() override;

    private:
        void checkChangeSelectedPagePreConditions(
            const QItemSelection& selected) const;

        void updateDisplayName(const QModelIndex& selectedIndex) const;

        void showSelectedPageWidget(QWidget* widget) const;

        void showEmptyPageWidget() const;

        void resetModifiedSettingsPages() const;

        SettingsPageRegistry& m_registry;
        SettingsDialogWeakPtr settingsDialog;
        std::shared_ptr<SettingsPageGroupTreeModel> treeModel;

        const LoggerPtr logger;
        SettingsPagePtr currentlySelectedPage{nullptr};

        QString m_currentSearchPattern;

        SettingsDialogGeometryAndState saveGeometryAndState;
    };
} // namespace aide::core

#endif // AIDE_SHOW_SETTINGS_DIALOG_HPP
