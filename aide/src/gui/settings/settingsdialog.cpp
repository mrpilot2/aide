#include "settingsdialog.hpp"

#include <utility>

#include <QPushButton>
#include <QWidget>

#include "ui_settingsdialog.h"

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
}

SettingsDialog::~SettingsDialog() = default;

void SettingsDialog::setController(SettingsDialogControllerPtr controller)
{
    settingsController = std::move(controller);
    connectSignals();
}

void SettingsDialog::connectSignals() {}

void SettingsDialog::executeDialog()
{
    this->exec();
}

void SettingsDialog::setTreeModel(std::shared_ptr<QAbstractItemModel> model)
{
    ui->treeView->setModel(model.get());
    ui->treeView->hideColumn(1);

    connect(ui->treeView->selectionModel(),
            &QItemSelectionModel::selectionChanged, settingsController.get(),
            &SettingsDialogController::onUserChangedSelectedPage);
}

void SettingsDialog::setSelectedPageDisplayName(const std::string& displayName)
{
    ui->displayNameLabel->setText(QString::fromStdString(displayName));
}
