

#include "thirdpartylicensesdialog.hpp"

#include "ui_thirdpartylicensesdialog.h"

using aide::gui::ThirdPartyLicensesDialog;

ThirdPartyLicensesDialog::ThirdPartyLicensesDialog(QWidget* parent)
    : QDialog(parent)
    , m_ui{std::make_unique<Ui::ThirdPartyLicensesDialog>()}
{
    m_ui->setupUi(this);
    m_ui->textBrowser->setOpenExternalLinks(true);
    QPalette palette;
    m_ui->textBrowser->setStyleSheet(
        QString("background-color: %1")
            .arg(palette.color(QPalette::Disabled, QPalette::Window).name()));
}

ThirdPartyLicensesDialog::~ThirdPartyLicensesDialog() = default;

void aide::gui::ThirdPartyLicensesDialog::setLicensesText(const QString& text)
{
    m_ui->textBrowser->setText(text);
    m_ui->textBrowser->adjustSize();
}
