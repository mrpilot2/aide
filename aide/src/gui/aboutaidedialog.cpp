
#include "aboutaidedialog.hpp"

#include <QClipboard>
#include <QString>
#include <QToolTip>

#include "aideinformation.hpp"
#include "diagnostictextbuilder.hpp"
#include "thirdpartylicensesdialog.hpp"
#include "ui_aboutaidedialog.h"

using aide::gui::AboutAideDialog;

AboutAideDialog::AboutAideDialog(QWidget* parent)
    : QDialog(parent)
    , m_ui{std::make_unique<Ui::AboutAideDialog>()}
{
    m_ui->setupUi(this);

    m_ui->info->setOpenExternalLinks(true);

    connect(m_ui->info, &QLabel::linkHovered, this,
            &AboutAideDialog::whatsNewHovered);
    connect(m_ui->thirdParty, &QLabel::linkActivated, this,
            &AboutAideDialog::onThirdPartyLibrariesLinkClicked);
    connect(m_ui->copyButton, &QPushButton::clicked, this,
            &AboutAideDialog::copySystemInfoToClipBoard);
}

AboutAideDialog::~AboutAideDialog() = default;

void AboutAideDialog::showAboutInformation(const core::AideInformation& info)
{
    const auto locale{QLocale::system()};
    const auto textToDisplay{
        QString("<h3>aIDE %1</h3><br>"
                "Build %2, %3 %4<br><br><a href=\"%5\">%6</a>")
            .arg(QString::fromStdString(info.versionInfo),
                 QString::fromStdString(info.gitHash), tr("built on"),
                 locale.toString(info.buildDate,
                                 QLocale::FormatType::LongFormat),
                 QString::fromStdString(info.whatsNewUrl), tr("What's new"))};

    m_ui->info->setText(textToDisplay);

    m_info = info;

    //    m_ui->textBrowser->setOpenExternalLinks(true);
    //    m_ui->textBrowser->setHtml(QString::fromStdString(info.thirdPartyLicensesHtml));

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

void AboutAideDialog::onThirdPartyLibrariesLinkClicked() const
{
    ThirdPartyLicensesDialog licensesDialog(m_ui->info);
    licensesDialog.setLicensesText(
        QString::fromStdString(m_info.thirdPartyLicensesHtml));

    licensesDialog.exec();
}

void AboutAideDialog::copySystemInfoToClipBoard() const
{
    auto* clipboard = QApplication::clipboard();
    clipboard->setText(core::DiagnosticTextBuilder::build(m_info));
}
