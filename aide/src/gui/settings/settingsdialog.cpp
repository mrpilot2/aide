#include "settingsdialog.hpp"

#include <utility>

#include <QList>
#include <QPushButton>
#include <QWidget>

#include "changedetector.hpp"
#include "treeitem.hpp"
#include "ui_settingsdialog.h"

using aide::core::SettingsDialogGeometryAndStateData;
using aide::core::TreeItem;
using aide::core::UserSelection;
using aide::gui::SettingsDialog;

SettingsDialog::SettingsDialog(QWidget* parent)
    : QDialog(parent)
    , ui(std::make_unique<Ui::SettingsDialog>())
{
    ui->setupUi(this);

    ui->leaveDialogButtonBox->button(QDialogButtonBox::Apply)
        ->setEnabled(false);
    ui->helpButtonBox->button(QDialogButtonBox::Help)->setEnabled(false);

    ui->treeView->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->treeView->header()->setStretchLastSection(false);
    ui->treeView->header()->setVisible(false);

    ui->resetLabel->hide();

    ui->splitter->setStretchFactor(0, 0);
    ui->splitter->setStretchFactor(1, 3);
}

SettingsDialog::~SettingsDialog()
{
    // NOLINTNEXTLINE
    delete ui->defaultScrollAreaWidget;
}

void SettingsDialog::setController(SettingsDialogControllerPtr controller)
{
    settingsController = std::move(controller);
    connectSignals();
}

void SettingsDialog::connectSignals()
{
    connect(ui->resetLabel, &QLabel::linkActivated, settingsController.get(),
            &SettingsDialogController::onUserWantsToResetCurrentPage);
    connect(ui->leaveDialogButtonBox->button(QDialogButtonBox::Apply),
            &QPushButton::clicked, settingsController.get(),
            &SettingsDialogController::onUserWantsToApplySettingsPages);
}

void SettingsDialog::restoreGeometryAndState(
    const SettingsDialogGeometryAndStateData geometryAndStateData)
{
    this->restoreGeometry(geometryAndStateData.dialogGeometry);
    ui->splitter->restoreGeometry(geometryAndStateData.splitterGeometry);
    ui->splitter->restoreState(geometryAndStateData.splitterState);
    ui->treeView->restoreGeometry(geometryAndStateData.treeViewGeometry);
}

void SettingsDialog::setSelectedGroupIndex(const QModelIndex& index)
{
    if (!index.isValid()) { return; }
    ui->treeView->selectionModel()->setCurrentIndex(
        index, QItemSelectionModel::SelectionFlag::SelectCurrent);
}

SettingsDialogGeometryAndStateData SettingsDialog::currentGeometry() const
{
    SettingsDialogGeometryAndStateData dialogGeometryAndStateData;
    dialogGeometryAndStateData.dialogGeometry   = this->saveGeometry();
    dialogGeometryAndStateData.splitterGeometry = ui->splitter->saveGeometry();
    dialogGeometryAndStateData.splitterState    = ui->splitter->saveState();
    dialogGeometryAndStateData.treeViewGeometry = ui->treeView->saveGeometry();

    auto selectedIndexes = ui->treeView->selectionModel()->selectedIndexes();
    if (!selectedIndexes.empty()) {
        const auto& index = selectedIndexes.at(0);

        if (index.isValid()) {
            const auto completeGroupIndex =
                ui->treeView->model()->index(index.row(), 0, index.parent());
            const auto* const treeItem =
                static_cast<TreeItem*>(completeGroupIndex.internalPointer());
            const auto completeGroupString{
                treeItem->getHiddenUserData().toString()};

            dialogGeometryAndStateData.selectedTreeViewItem =
                completeGroupString;
        }
    }

    return dialogGeometryAndStateData;
}

UserSelection SettingsDialog::executeDialog()
{
    return this->exec() == QDialog::Accepted ? UserSelection::Ok
                                             : UserSelection::Cancel;
}

void SettingsDialog::setTreeModel(
    const std::shared_ptr<QAbstractItemModel> model)
{
    ui->treeView->setModel(model.get());
    ui->treeView->resizeColumnToContents(0);

    connect(ui->treeView->selectionModel(),
            &QItemSelectionModel::selectionChanged, settingsController.get(),
            &SettingsDialogController::onUserChangedSelectedPage);
}

void SettingsDialog::setSelectedPageDisplayName(const std::string& displayName)
{
    ui->displayNameLabel->setText(QString::fromStdString(displayName));
}

void SettingsDialog::showSelectedPageWidget(QWidget* widget)
{
    auto* oldWidget = ui->settingsPageScrollArea->takeWidget();

    ui->settingsPageScrollArea->setWidget(widget);

    if (oldWidget != widget) {
        unInstallChangeDetector(oldWidget);
        installChangeDetector(widget);
    }
}

void SettingsDialog::showEmptyPageWidget()
{
    auto* oldWidget = ui->settingsPageScrollArea->takeWidget();

    ui->settingsPageScrollArea->setWidget(ui->defaultScrollAreaWidget);

    if (oldWidget != ui->defaultScrollAreaWidget) {
        unInstallChangeDetector(oldWidget);
        installChangeDetector(ui->defaultScrollAreaWidget);
    }
}

void SettingsDialog::showResetLabel(const bool show)
{
    ui->resetLabel->setVisible(show);
}

void SettingsDialog::enableApplyButton(const bool enable)
{
    ui->leaveDialogButtonBox->button(QDialogButtonBox::Apply)
        ->setEnabled(enable);
}

void SettingsDialog::installChangeDetector(QObject* widget) const
{
    gui::installChangeDetector(widget, settingsController);
}

void SettingsDialog::unInstallChangeDetector(QObject* widget) const
{
    gui::unInstallChangeDetector(widget, settingsController);
}
