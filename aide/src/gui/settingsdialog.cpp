#include "settingsdialog.hpp"

#include <QPushButton>
#include <QWidget>

#include "ui_settingsdialog.h"

using aide::gui::SettingsDialog;

SettingsDialog::SettingsDialog(QWidget* parent)
    : QDialog(parent)
    , m_ui(std::make_unique<Ui::SettingsDialog>())
{
    m_ui->setupUi(this);

    m_ui->leaveDialogButtonBox->button(QDialogButtonBox::Apply)
        ->setEnabled(false);
    m_ui->helpButtonBox->button(QDialogButtonBox::Help)->setEnabled(false);
}

SettingsDialog::~SettingsDialog() = default;
