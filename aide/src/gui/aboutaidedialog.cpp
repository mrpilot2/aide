
#include "aboutaidedialog.hpp"

#include <QString>
#include <QToolTip>

#include "aideinformation.hpp"
#include "ui_aboutaidedialog.h"

using aide::gui::AboutAideDialog;

AboutAideDialog::AboutAideDialog(QWidget* parent)
    : QDialog(parent)
    , m_ui{std::make_unique<Ui::AboutDialog>()}
{
    m_ui->setupUi(this);

    m_ui->info->setOpenExternalLinks(true);

    connect(m_ui->info, &QLabel::linkHovered, this,
            &AboutAideDialog::whatsNewHovered);
}

AboutAideDialog::~AboutAideDialog() = default;

void AboutAideDialog::showAboutInformation(
    const aide::core::AideInformation& info)
{
    const auto locale{QLocale::system()};
    const auto textToDisplay{
        QString("<h3>aIDE %1</h3><br>"
                "Build %2, built on %3<br><br><a href=\"%4\">What's new</a>")
            .arg(QString::fromStdString(info.versionInfo),
                 QString::fromStdString(info.gitHash),
                 locale.toString(info.buildDate,
                                 QLocale::FormatType::LongFormat),
                 QString::fromStdString(info.whatsNewBaseUrl) + "v" +
                     QString::fromStdString(info.versionInfo))};

    m_ui->info->setText(textToDisplay);

    this->layout()->setSizeConstraint(QLayout::SetFixedSize);

    this->exec();
}

void AboutAideDialog::whatsNewHovered(const QString& text)
{
    if (text.isEmpty()) {
        QToolTip::hideText();
        return;
    }

    QToolTip::showText(QCursor::pos(), text);
}
