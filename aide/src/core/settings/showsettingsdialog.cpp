#include "showsettingsdialog.hpp"

#include <stdexcept>
#include <utility>

#include <QItemSelection>

#include "settings/settingspage.hpp"
#include "settings/settingspageregistry.hpp"

using aide::core::SettingsPageGroupTreeModel;
using aide::core::ShowSettingsDialog;

aide::core::ShowSettingsDialog::ShowSettingsDialog(
    aide::core::SettingsDialogWeakPtr dialog, LoggerPtr loggerInterface)
    : settingsDialog{std::move(dialog)}
    , logger{std::move(loggerInterface)}
{}

void ShowSettingsDialog::showSettingsDialog()
{
    auto dialog = settingsDialog.lock();

    if (dialog == nullptr) { return; }

    treeModel = std::make_shared<SettingsPageGroupTreeModel>();
    dialog->setTreeModel(treeModel);
    dialog->executeDialog();
}

void ShowSettingsDialog::changeSelectedPage(
    const QItemSelection& selected,
    [[maybe_unused]] const QItemSelection& deselected)
{
    checkChangeSelectedPagePreConditions(selected);

    auto selectedIndex = selected.indexes().at(0);

    updateDisplayName(selectedIndex);

    const auto settingsPage = findCorrespondingSettingsPage(selectedIndex);

    if (settingsPage != nullptr) {
        showSelectedPageWidget(settingsPage->widget());
    } else {
        showEmptyPageWidget();
    }
}

void ShowSettingsDialog::checkChangeSelectedPagePreConditions(
    const QItemSelection& selected) const
{
    if (selected.indexes().empty()) {
        const auto* message{
            "ShowSettingsDialog: selected group index (QItemSelection) is "
            "invalid. This should never happen."};
        logger->critical(message);
        throw std::invalid_argument(message);
    }

    if (treeModel == nullptr) {
        const auto* message{
            "ShowSettingsDialog: Tree Model is nullptr. This can only happen, "
            "if changeSelectedPage() was triggered before the user requested "
            "the dialog to show"};
        logger->critical(message);
        throw std::logic_error(message);
    }
}

aide::core::SettingsPagePtr ShowSettingsDialog::findCorrespondingSettingsPage(
    const QModelIndex& selectedIndex) const
{
    auto completeGroupIndex =
        treeModel->index(selectedIndex.row(), 1, selectedIndex.parent());
    auto completeGroupName{treeModel->data(completeGroupIndex, Qt::DisplayRole)
                               .toString()
                               .toStdString()};

    const auto& pages = SettingsPageRegistry::settingsPages();

    if (auto it = std::find_if(pages.begin(), pages.end(),
                               [completeGroupName](const auto& page) {
                                   return page->group().name() ==
                                          completeGroupName;
                               });
        it != pages.end()) {
        return *it;
    }
    return nullptr;
}

void ShowSettingsDialog::updateDisplayName(
    const QModelIndex& selectedIndex) const
{
    auto completeGroupIndex =
        treeModel->index(selectedIndex.row(), 1, selectedIndex.parent());
    auto completeGroupString{
        treeModel->data(completeGroupIndex, Qt::DisplayRole)
            .toString()
            .toStdString()};

    logger->trace("User changed settings page to {} ", completeGroupString);

    if (auto d = settingsDialog.lock(); d != nullptr) {
        d->setSelectedPageDisplayName(completeGroupString);
    }
}

void ShowSettingsDialog::showSelectedPageWidget(QWidget* widget) const
{
    if (auto d = settingsDialog.lock(); d != nullptr) {
        d->showSelectedPageWidget(widget);
    }
}

void ShowSettingsDialog::showEmptyPageWidget() const
{
    if (auto d = settingsDialog.lock(); d != nullptr) {
        d->showEmptyPageWidget();
    }
}
