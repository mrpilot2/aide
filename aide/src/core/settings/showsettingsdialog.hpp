#ifndef AIDE_SHOW_SETTINGS_DIALOG_HPP
#define AIDE_SHOW_SETTINGS_DIALOG_HPP

#include <utility>
#include <vector>

#include <QModelIndex>
#include <QString>

#include <settings/settingspage.hpp>
#include <settings/settingspageregistry.hpp>

#include "loggerinterface.hpp"
#include "searchhistory.hpp"
#include "settingsdialogchangepagecontroller.hpp"
#include "settingsdialoggeometryandstate.hpp"
#include "settingsdialoginterface.hpp"
#include "settingspagefilterproxymodel.hpp"
#include "settingspagegrouptreemodel.hpp"
#include "settingspageranker.hpp"
#include "showhelpusecase.hpp"
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

        // With a help use case, the dialog's Help button opens the selected
        // page's Help Topic. Without one, the button stays hidden.
        ShowSettingsDialog(SettingsDialogWeakPtr dialog,
                           SettingsPageRegistry& registry,
                           SettingsInterface& settings,
                           LoggerPtr loggerInterface,
                           const ShowHelpUseCase& helpUseCase);

        void showSettingsDialog() override;

        void showSettingsDialog(
            const aide::HierarchicalId& selectedGroup) override;

        void changeSelectedPage(const QItemSelection& selected,
                                const QItemSelection& deselected) override;

        void searchPatternChanged(const QString& pattern) override;

        void commitCurrentSearchPattern() override;

        [[nodiscard]] const QString& currentSearchPattern() const;

        void anyGuiElementHasChanged() override;

        void resetCurrentPage() override;

        void applyModifiedSettingsPages() override;

        void showHelpForCurrentPage() override;

    private:
        // Shared implementation for both overloads above; nullptr means "use
        // the persisted last-selected page", matching the original no-arg
        // behavior.
        void showSettingsDialog(
            const aide::HierarchicalId* explicitlySelectedGroup);

        ShowSettingsDialog(SettingsDialogWeakPtr dialog,
                           SettingsPageRegistry& registry,
                           SettingsInterface& settings,
                           LoggerPtr loggerInterface,
                           const ShowHelpUseCase* helpUseCase);

        void checkTreeModelIsInitialized() const;

        bool autoSelectBestMatchingPage();

        void collectVisiblePages(
            const QModelIndex& proxyParent,
            std::vector<std::pair<QModelIndex, SettingsPagePtr>>& out) const;

        void clearSelectedPage();

        [[nodiscard]] bool isHelpActive() const;

        [[nodiscard]] bool isHelpAvailableForCurrentPage() const;

        void updateDisplayName(const QModelIndex& selectedIndex) const;

        void showSelectedPageWidget(QWidget* widget) const;

        void showEmptyPageWidget() const;

        void resetModifiedSettingsPages() const;

        [[nodiscard]] QModelIndex mapToSourceIndex(
            const QModelIndex& index) const;

        [[nodiscard]] QModelIndex mapFromSourceIndex(
            const QModelIndex& sourceIndex) const;

        SettingsPageRegistry& m_registry;
        SettingsDialogWeakPtr settingsDialog;
        std::shared_ptr<SettingsPageGroupTreeModel> treeModel;
        std::shared_ptr<SettingsPageFilterProxyModel> proxyModel;

        const LoggerPtr logger;
        const ShowHelpUseCase* m_helpUseCase;
        SettingsPagePtr currentlySelectedPage{nullptr};

        QString m_currentSearchPattern;

        SettingsDialogGeometryAndState saveGeometryAndState;

        SearchHistory searchHistory;
    };
} // namespace aide::core

#endif // AIDE_SHOW_SETTINGS_DIALOG_HPP
