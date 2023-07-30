
#include "aboutaidedialog.hpp"

#include <QClipboard>
#include <QString>
#include <QThread>
#include <QToolTip>

#include "aideinformation.hpp"
#include "systemmemory.hpp"
#include "ui_aboutaidedialog.h"

using aide::gui::AboutAideDialog;
using aide::utils::SystemMemory;

AboutAideDialog::AboutAideDialog(QWidget* parent)
    : QDialog(parent)
    , m_ui{std::make_unique<Ui::AboutDialog>()}
{
    m_ui->setupUi(this);

    m_ui->info->setOpenExternalLinks(true);

    connect(m_ui->info, &QLabel::linkHovered, this,
            &AboutAideDialog::whatsNewHovered);
    connect(m_ui->copyButton, &QPushButton::clicked, this,
            &AboutAideDialog::copySystemInfoToClipBoard);
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
                 QString::fromStdString(info.whatsNewUrl))};

    m_ui->info->setText(textToDisplay);

    m_info = info;

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

void AboutAideDialog::copySystemInfoToClipBoard() const
{
    auto memoryInBytes{SystemMemory::getAvailableRAMInBytes()};
    auto memoryInfo{QString("%1 MB").arg(
        memoryInBytes.has_value()
            ? QString::number(memoryInBytes.value() / BYTE_TO_MEGABYTE)
            : "Undefined")};

    const auto locale{QLocale::system()};
    auto clipBoardText{
        QString("aIDE %1\n%2 built on %3\nQt Version: %4\nOS: %5\nKernel: "
                "%6\nMemory: %7\nCores: %8\n")
            .arg(QString::fromStdString(m_info.versionInfo),
                 QString::fromStdString(m_info.gitHash),
                 locale.toString(m_info.buildDate,
                                 QLocale::FormatType::LongFormat),
                 qVersion(), QSysInfo::prettyProductName(),
                 QSysInfo::kernelVersion(), memoryInfo,
                 QString::number(QThread::idealThreadCount()))};

#ifdef Q_OS_LINUX
    clipBoardText += QString("Desktop environment: %1")
                         .arg(QString(qgetenv("XDG_CURRENT_DESKTOP")));
#endif

    auto* clipboard = QApplication::clipboard();
    clipboard->setText(clipBoardText);
}
