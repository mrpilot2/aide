#include "vcsprocessrunner.hpp"

#include <utility>

#include <QProcess>
#include <QProcessEnvironment>
#include <QString>
#include <QStringList>

#include "vcscommandlineredactor.hpp"

using aide::VcsProcessResult;
using aide::VcsProcessRunner;

namespace
{
    QStringList toQStringList(const std::vector<std::string>& arguments)
    {
        QStringList result;
        result.reserve(static_cast<qsizetype>(arguments.size()));
        for (const auto& argument : arguments) {
            result << QString::fromStdString(argument);
        }
        return result;
    }

    std::string buildLoggedCommandLine(
        const std::string& executable,
        const std::vector<std::string>& arguments)
    {
        std::string commandLine = executable;
        for (const auto& argument : arguments) {
            commandLine += ' ';
            commandLine += argument;
        }
        return aide::redactVcsCommandLineCredentials(commandLine);
    }
} // namespace

VcsProcessRunner::VcsProcessRunner(LoggerPtr logger, QObject* parent)
    : QObject(parent)
    , m_logger(std::move(logger))
{}

void VcsProcessRunner::run(const std::string& workingDirectory,
                           const std::string& executable,
                           const std::vector<std::string>& arguments,
                           ResultCallback callback)
{
    if (m_logger) {
        m_logger->info("{}", buildLoggedCommandLine(executable, arguments));
    }

    auto* process = new QProcess(this);
    process->setWorkingDirectory(QString::fromStdString(workingDirectory));

    // git localizes its stderr messages; VcsRepository::classifyError()
    // matches against the English text (e.g. "not a git repository"), so
    // force the C locale to keep that classification independent of the
    // user's environment.
    auto environment = QProcessEnvironment::systemEnvironment();
    environment.insert("LC_ALL", "C");
    environment.insert("LANGUAGE", "C");
    process->setProcessEnvironment(environment);

    QObject::connect(process, &QProcess::errorOccurred, process,
                     [process, callback](QProcess::ProcessError error) {
                         if (error != QProcess::FailedToStart) { return; }
                         process->deleteLater();
                         callback(VcsProcessResult{.exitCode       = -1,
                                                   .crashed        = true,
                                                   .standardOutput = {},
                                                   .standardError  = {}});
                     });

    QObject::connect(
        process,
        static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(
            &QProcess::finished),
        process,
        [process, callback](int exitCode, QProcess::ExitStatus exitStatus) {
            const VcsProcessResult result{
                .exitCode = exitCode,
                .crashed  = exitStatus == QProcess::CrashExit,
                .standardOutput =
                    process->readAllStandardOutput().toStdString(),
                .standardError = process->readAllStandardError().toStdString()};
            process->deleteLater();
            callback(result);
        });

    process->start(QString::fromStdString(executable),
                   toQStringList(arguments));
}
