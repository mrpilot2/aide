#include "reportbugpreviewlauncher.hpp"

#include <string>

#include <QApplication>
#include <QMessageBox>
#include <QString>
#include <QStringList>
#include <QUrl>
#include <QUrlQuery>

using demo::ReportBugPreviewLauncher;

bool ReportBugPreviewLauncher::openUrl(const std::string& url) const
{
    const QUrl requestedUrl(QString::fromStdString(url));
    const QUrlQuery query(requestedUrl);

    QStringList fields;
    for (const auto& item : query.queryItems(QUrl::FullyDecoded)) {
        fields << QString("%1:\n%2").arg(item.first, item.second);
    }

    QMessageBox::information(
        nullptr, QApplication::tr("Report Bug in aIDE (preview)"),
        QApplication::tr("The demo app does not file real GitHub issues. "
                         "This would have opened a new issue against "
                         "aIDE's own repository (%1) with:\n\n%2")
            .arg(requestedUrl.toString(QUrl::RemoveQuery),
                 fields.join("\n\n")));

    return true;
}
