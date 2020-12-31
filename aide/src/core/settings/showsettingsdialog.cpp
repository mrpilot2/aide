#include "showsettingsdialog.hpp"

#include <stdexcept>
#include <utility>

#include <QItemSelection>

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

    auto index = selected.indexes().at(0);

    auto completeGroupIndex = treeModel->index(index.row(), 1, index.parent());
    auto completeGroupString{
        treeModel->data(completeGroupIndex, Qt::DisplayRole)
            .toString()
            .toStdString()};

    logger->trace("User changed settings page to {} ", completeGroupString);

    if (auto d = settingsDialog.lock(); d != nullptr) {
        d->setSelectedPageDisplayName(completeGroupString);
    }
}
