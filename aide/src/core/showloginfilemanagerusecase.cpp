
#include "showloginfilemanagerusecase.hpp"

#include <utility>

#include <QFileInfo>
#include <QString>

#include "logger/loggerfactory.hpp"

using aide::core::ShowLogInFileManagerUseCase;

ShowLogInFileManagerUseCase::ShowLogInFileManagerUseCase(
    FileManagerLauncherPtr launcher, LoggerPtr logger)
    : m_launcher(std::move(launcher))
    , m_logger(std::move(logger))
{}

void ShowLogInFileManagerUseCase::showLogInFileManager() const
{
    const auto logFilePath = LoggerFactory::logFilePath();

    if (!logFilePath) {
        m_logger->warn(
            "Could not resolve the log file path; unable to show log in "
            "file manager");
        return;
    }

    const auto logDirectory = QFileInfo(QString::fromStdString(*logFilePath))
                                  .absolutePath()
                                  .toStdString();

    if (!m_launcher->openDirectory(logDirectory)) {
        m_logger->warn("Could not open the file manager at log directory {}",
                       logDirectory);
    }
}
