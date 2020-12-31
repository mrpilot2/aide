#include "settingsdialog.hpp"

#include <utility>

#include <QList>
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

    ui->treeView->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->treeView->header()->setStretchLastSection(false);
    ui->treeView->header()->setVisible(false);
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
}

void SettingsDialog::showEmptyPageWidget()
{
    ui->settingsPageScrollArea->takeWidget();

    ui->settingsPageScrollArea->setWidget(ui->defaultScrollAreaWidget);
}
