#include "settingsdialog.hpp"

#include <utility>

#include <QList>
#include <QPushButton>
#include <QWidget>

#include "changedetector.hpp"
#include "ui_settingsdialog.h"

using aide::core::UserSelection;
using aide::gui::SettingsDialog;
using aide::gui::SettingsDialogController;

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
}

SettingsDialog::~SettingsDialog() = default;

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

UserSelection SettingsDialog::executeDialog()
{
    return (this->exec() == QDialog::Accepted) ? UserSelection::Ok
                                               : UserSelection::Cancel;
}

void SettingsDialog::setTreeModel(std::shared_ptr<QAbstractItemModel> model)
{
    ui->treeView->setModel(model.get());
    ui->treeView->hideColumn(1);
    ui->treeView->resizeColumnToContents(0);
    ui->splitter->setSizes({ui->treeView->size().width(),
                            ui->settingsPageLayout->sizeHint().width()});

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
    ui->settingsPageScrollArea->takeWidget();

    ui->settingsPageScrollArea->setWidget(widget);

    installChangeDetector(widget);
}

void SettingsDialog::showEmptyPageWidget()
{
    ui->settingsPageScrollArea->takeWidget();

    ui->settingsPageScrollArea->setWidget(ui->defaultScrollAreaWidget);
}

void SettingsDialog::showResetLabel(bool show)
{
    ui->resetLabel->setVisible(show);
}

void SettingsDialog::enableApplyButton(bool enable)
{
    ui->leaveDialogButtonBox->button(QDialogButtonBox::Apply)
        ->setEnabled(enable);
}

void SettingsDialog::installChangeDetector(QObject* widget)
{
    aide::gui::installChangeDetector(widget, settingsController);
}
