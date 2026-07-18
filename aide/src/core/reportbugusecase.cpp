
#include "reportbugusecase.hpp"

#include <utility>

#include <QString>
#include <QUrl>
#include <QUrlQuery>

#include <aide/githubrepository.hpp>

#include "aideinformation.hpp"
#include "aideinformationbuilder.hpp"
#include "diagnostictextbuilder.hpp"

using aide::core::ReportBugUseCase;

namespace
{
    constexpr auto LOGS_PLACEHOLDER = "Paste your log here";
    constexpr auto LOGS_SEPARATOR   = "\n\n---\n\n";
} // namespace

ReportBugUseCase::ReportBugUseCase(UrlLauncherPtr launcher, LoggerPtr logger)
    : m_launcher(std::move(launcher))
    , m_logger(std::move(logger))
{}

void ReportBugUseCase::reportBug() const
{
    const auto info = AideInformationBuilder::buildCurrent();

    const auto logs = QString(LOGS_PLACEHOLDER) + QString(LOGS_SEPARATOR) +
                      DiagnosticTextBuilder::build(info);

    QUrl url(QString(constants::GITHUB_REPO_URL) + "/issues/new");
    QUrlQuery query;
    query.addQueryItem("template", "bug.yml");
    query.addQueryItem("version", QString::fromStdString(info.versionInfo));
    query.addQueryItem("logs", logs);
    url.setQuery(query);

    m_logger->trace("Reporting bug at {}", url.toString().toStdString());

    if (!m_launcher->openUrl(url.toString(QUrl::FullyEncoded).toStdString())) {
        m_logger->warn("Could not open the browser to report a bug");
    }
}
