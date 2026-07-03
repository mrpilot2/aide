#include "showsettingsdialog.hpp"

#include <stdexcept>
#include <utility>

#include <QItemSelection>
#include <QPersistentModelIndex>
#include <QWidget>

#include "settings/settingspage.hpp"

using aide::core::SettingsPageGroupTreeModel;
using aide::core::ShowSettingsDialog;

ShowSettingsDialog::ShowSettingsDialog(SettingsDialogWeakPtr dialog,
                                       SettingsPageRegistry& registry,
                                       SettingsInterface& settings,
                                       LoggerPtr loggerInterface)
    : m_registry(registry)
    , settingsDialog{std::move(dialog)}
    , logger{std::move(loggerInterface)}
    , saveGeometryAndState(settingsDialog, settings)
{}

void ShowSettingsDialog::showSettingsDialog()
{
    const auto dialog = settingsDialog.lock();

    if (dialog == nullptr) { return; }

    dialog->showEmptyPageWidget();
    dialog->setSelectedPageDisplayName("");

    treeModel  = std::make_shared<SettingsPageGroupTreeModel>(m_registry);
    proxyModel = std::make_shared<SettingsPageFilterProxyModel>();
    proxyModel->setSourceModel(treeModel.get());
    proxyModel->setSearchPattern(m_currentSearchPattern);
    dialog->setTreeModel(proxyModel);

    saveGeometryAndState.restoreGeometryAndState();

    const auto& pages = m_registry.settingsPages();
    std::ranges::for_each(pages, [](const auto& p) {
        (*p).reset();
        p->widget()->setVisible(false);
    });

    const auto lastSelectedTreeItem =
        saveGeometryAndState.selectedTreeViewItem();

    if (!lastSelectedTreeItem.isEmpty()) {
        const auto index = treeModel->recursivelyFindSelectedTreeItemIndex(
            lastSelectedTreeItem, QModelIndex());

        dialog->setSelectedGroupIndex(mapFromSourceIndex(index));
    } else {
        dialog->setSelectedGroupIndex(
            mapFromSourceIndex(treeModel->index(0, 0, QModelIndex())));
    }

    const auto result = dialog->executeDialog();

    saveGeometryAndState.saveGeometryAndState(dialog->currentGeometry());

    if (result == UserSelection::Ok) {
        applyModifiedSettingsPages();
    } else {
        resetModifiedSettingsPages();
    }
}

void ShowSettingsDialog::changeSelectedPage(
    const QItemSelection& selected,
    [[maybe_unused]] const QItemSelection& deselected)
{
    checkChangeSelectedPagePreConditions(selected);

    const auto selectedIndex = mapToSourceIndex(selected.indexes().at(0));

    if (const auto view = settingsDialog.lock(); view != nullptr) {
        updateDisplayName(selectedIndex);

        currentlySelectedPage =
            treeModel->findCorrespondingSettingsPage(selectedIndex);

        if (currentlySelectedPage != nullptr) {
            showSelectedPageWidget(currentlySelectedPage->widget());
            view->showResetLabel(currentlySelectedPage->isModified());
            currentlySelectedPage->highlight(m_currentSearchPattern);
        } else {
            showEmptyPageWidget();
            view->showResetLabel(false);
        }
    }
}

void ShowSettingsDialog::searchPatternChanged(const QString& pattern)
{
    m_currentSearchPattern = pattern;

    if (proxyModel != nullptr) { proxyModel->setSearchPattern(pattern); }

    if (currentlySelectedPage != nullptr) {
        currentlySelectedPage->highlight(pattern);
    }

    logger->trace("User changed settings search pattern to {} ",
                  pattern.toStdString());
}

QModelIndex ShowSettingsDialog::mapToSourceIndex(const QModelIndex& index) const
{
    if (proxyModel != nullptr && index.model() == proxyModel.get()) {
        return proxyModel->mapToSource(index);
    }
    return index;
}

QModelIndex ShowSettingsDialog::mapFromSourceIndex(
    const QModelIndex& sourceIndex) const
{
    if (proxyModel != nullptr && sourceIndex.isValid()) {
        return proxyModel->mapFromSource(sourceIndex);
    }
    return sourceIndex;
}

const QString& ShowSettingsDialog::currentSearchPattern() const
{
    return m_currentSearchPattern;
}

void ShowSettingsDialog::checkChangeSelectedPagePreConditions(
    const QItemSelection& selected) const
{
    if (selected.indexes().empty()) {
        constexpr auto message{
            "ShowSettingsDialog: selected group index (QItemSelection) is "
            "invalid. This should never happen."};
        logger->critical(message);
        throw std::invalid_argument(message);
    }

    if (treeModel == nullptr) {
        constexpr auto message{
            "ShowSettingsDialog: Tree Model is nullptr. This can only happen, "
            "if changeSelectedPage() was triggered before the user requested "
            "the dialog to show"};
        logger->critical(message);
        throw std::logic_error(message);
    }
}

void ShowSettingsDialog::updateDisplayName(
    const QModelIndex& selectedIndex) const
{
    const auto completeGroupIndex =
        treeModel->index(selectedIndex.row(), 0, selectedIndex.parent());
    const auto* treeItem =
        static_cast<TreeItem*>(completeGroupIndex.internalPointer());
    const auto completeGroupString{treeItem->getHiddenUserData()
                                       .toString()
                                       .replace("/", " > ")
                                       .toStdString()};

    logger->trace("User changed settings page to {} ", completeGroupString);

    if (const auto d = settingsDialog.lock(); d != nullptr) {
        d->setSelectedPageDisplayName(completeGroupString);
    }
}

void ShowSettingsDialog::showSelectedPageWidget(QWidget* widget) const
{
    if (const auto d = settingsDialog.lock(); d != nullptr) {
        d->showSelectedPageWidget(widget);
    }
}

void ShowSettingsDialog::showEmptyPageWidget() const
{
    if (const auto d = settingsDialog.lock(); d != nullptr) {
        d->showEmptyPageWidget();
    }
}

void ShowSettingsDialog::anyGuiElementHasChanged()
{
    if (const auto d = settingsDialog.lock();
        d != nullptr && currentlySelectedPage != nullptr) {
        d->showResetLabel(currentlySelectedPage->isModified());
        const auto& pages = m_registry.settingsPages();

        d->enableApplyButton(std::ranges::any_of(
            pages, [](const auto& p) { return p->isModified(); }));
    }
}

void ShowSettingsDialog::resetCurrentPage()
{
    if (currentlySelectedPage != nullptr) {
        (*currentlySelectedPage).reset();
        anyGuiElementHasChanged();
    }
}

void ShowSettingsDialog::resetModifiedSettingsPages() const
{
    const auto& pages = m_registry.settingsPages();
    for (const auto& page : pages) {
        if (page->isModified()) { (*page).reset(); }
    }
}

void ShowSettingsDialog::applyModifiedSettingsPages()
{
    const auto& pages = m_registry.settingsPages();
    for (const auto& page : pages) {
        if (page->isModified()) {
            page->apply();
            if (page == currentlySelectedPage) { anyGuiElementHasChanged(); }
        }
    }
}
