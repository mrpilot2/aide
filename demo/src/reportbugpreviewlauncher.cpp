#include "reportbugpreviewlauncher.hpp"

#include <string>
#include <utility>

#include <QApplication>
#include <QMessageBox>
#include <QString>
#include <QStringList>
#include <QUrl>
#include <QUrlQuery>

#include <aide/githubrepository.hpp>

using demo::ReportBugPreviewLauncher;

ReportBugPreviewLauncher::ReportBugPreviewLauncher(aide::UrlLauncherPtr browser)
    : m_browser(std::move(browser))
{}

bool ReportBugPreviewLauncher::openUrl(const std::string& url) const
{
    const QUrl requestedUrl(QString::fromStdString(url));
    const QUrl newIssueUrl(QString(aide::constants::GITHUB_REPO_URL) +
                           QStringLiteral("/issues/new"));
    if (requestedUrl.adjusted(QUrl::RemoveQuery | QUrl::RemoveFragment) !=
        newIssueUrl) {
        return m_browser != nullptr && m_browser->openUrl(url);
    }

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
