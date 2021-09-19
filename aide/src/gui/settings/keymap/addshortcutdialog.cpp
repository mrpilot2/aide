
#include "addshortcutdialog.hpp"

#include "ui_addshortcutdialog.h"

using aide::gui::AddShortcutDialog;

AddShortcutDialog::AddShortcutDialog(QWidget* parent)
    : QDialog(parent)
    , ui{std::make_unique<Ui::AddShortcutDialog>()}
{
    ui->setupUi(this);

    ui->duplicateAssingmentGroupBox->hide();

    this->update();
}

AddShortcutDialog::~AddShortcutDialog() = default;

void AddShortcutDialog::setAssignedActionDescription(const QString& text)
{
    ui->shortCutNameLabel->setText(text);
}

QKeySequence AddShortcutDialog::userSelectedKeySequence() const
{
    return ui->keySequenceEdit->keySequence();
}
