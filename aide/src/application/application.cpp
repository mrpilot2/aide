
#include "application.hpp"

#include <QDir>
#include <QMainWindow>
#include <QStandardPaths>
#include <QString>

using aide::Application;
using aide::Logger;

// NOLINTNEXTLINE
Application::Application(int argc, char* argv[])
    : QApplication(argc, argv)
    , m_mainWindow(new QMainWindow(nullptr))
{
    if (!isOrganizationNameSet()) {
        throw std::runtime_error(
            "Application name and organization name need to be set before "
            "creating an aide::Application. This ensures that the logger uses "
            "a meaningful file path and that settings are stored in a "
            "meaningful location.");
    }

    setupLogger();

    m_mainWindow->showMaximized();
}

std::shared_ptr<Logger> aide::Application::logger() const
{
    return m_logger;
}

bool Application::isOrganizationNameSet()
{
    return !organizationName().isEmpty();
}

void Application::setupLogger()
{
    QString logLocation(
        QStandardPaths::writableLocation(QStandardPaths::CacheLocation));

    bool success = tryToCreateLogLocationIfItDoesNotExist(logLocation);

    if (!success) {
        logLocation =
            QStandardPaths::writableLocation(QStandardPaths::TempLocation);
        success = tryToCreateLogLocationIfItDoesNotExist(logLocation);
    }

    if (success) {
        FileName logPath(logLocation.append("/")
                             .append(applicationName())
                             .append(".log")
                             .toStdString());

        m_logger = std::make_shared<aide::Logger>(logPath);

        m_logger->info("Configured logger to log to file {}", logPath());
    } else {
        m_logger = std::make_shared<Logger>();
    }
}
bool Application::tryToCreateLogLocationIfItDoesNotExist(
    const QString& logLocation)
{
    if (logLocation.isEmpty()) {
        QDir d;
        if (!d.mkpath(logLocation)) {
            std::cerr << "Could not create standard log directory: "
                      << logLocation.toStdString()
                      << ". Trying to log into temp directory instead. If this "
                         "also does not work no file logging will happen.";
            return false;
        }
        return true;
    }
    return true;
}
