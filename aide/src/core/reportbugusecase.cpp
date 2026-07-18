
#include "reportbugusecase.hpp"

#include <utility>

#include <QString>
#include <QSysInfo>
#include <QUrl>
#include <QUrlQuery>

#include <aide/githubrepository.hpp>

#include "aideinformation.hpp"
#include "aideinformationbuilder.hpp"
#include "diagnostictextbuilder.hpp"
#include "logger/loggerfactory.hpp"

using aide::core::ReportBugUseCase;

namespace
{
    // Maps the current OS to one of the "Operating System" dropdown's fixed
    // option labels in bug.yml (Linux/Windows/MAC), so GitHub pre-selects
    // the matching option. Anything that is not recognizably Windows or
    // macOS is reported as Linux, the most common case for the remaining
    // kernels aIDE runs on.
    QString resolveOsOptionLabel()
    {
        if (QSysInfo::kernelType() == QLatin1String("winnt")) {
            return QStringLiteral("Windows");
        }
        if (QSysInfo::kernelType() == QLatin1String("darwin")) {
            return QStringLiteral("MAC");
        }
        return QStringLiteral("Linux");
    }
} // namespace

ReportBugUseCase::ReportBugUseCase(UrlLauncherPtr launcher, LoggerPtr logger)
    : m_launcher(std::move(launcher))
    , m_logger(std::move(logger))
{}

void ReportBugUseCase::reportBug() const
{
    const auto logFilePath = LoggerFactory::logFilePath();

    if (!logFilePath) {
        m_logger->warn(
            "Could not resolve the log file path; unable to report a bug");
        return;
    }

    const auto info = AideInformationBuilder::buildCurrent();

    const auto logs = DiagnosticTextBuilder::build(info) +
                      "\n\nLog file: " + QString::fromStdString(*logFilePath);

    QUrl url(QString(constants::GITHUB_REPO_URL) + "/issues/new");
    QUrlQuery query;
    query.addQueryItem("template", "bug.yml");
    query.addQueryItem("version", QString::fromStdString(info.versionInfo));
    query.addQueryItem("os", resolveOsOptionLabel());
    query.addQueryItem("logs", logs);
    url.setQuery(query);

    m_logger->trace("Reporting bug at {}", url.toString().toStdString());

    if (!m_launcher->openUrl(url.toString(QUrl::FullyEncoded).toStdString())) {
        m_logger->warn("Could not open the browser to report a bug");
    }
}
