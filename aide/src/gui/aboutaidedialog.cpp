
#include "aboutaidedialog.hpp"

#include <QString>

#include "aideinformation.hpp"
#include "ui_aboutaidedialog.h"

using aide::gui::AboutAideDialog;

AboutAideDialog::AboutAideDialog(QWidget* parent)
    : QDialog(parent)
    , m_ui{std::make_unique<Ui::AboutDialog>()}
{
    m_ui->setupUi(this);
}

AboutAideDialog::~AboutAideDialog() = default;

void AboutAideDialog::showAboutInformation(
    const aide::core::AideInformation& info)
{
    const auto locale{QLocale::system()};
    const auto textToDisplay{
        QString("<h3>aIDE %1</h3><br>"
                "Build %2, built on %3")
            .arg(QString::fromStdString(info.versionInfo),
                 QString::fromStdString(info.gitHash),
                 locale.toString(info.buildDate,
                                 QLocale::FormatType::LongFormat))};

    m_ui->info->setText(textToDisplay);

    this->exec();
}
