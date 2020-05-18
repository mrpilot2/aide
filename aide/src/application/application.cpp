
#include "application.hpp"

#include <QDir>
#include <QStandardPaths>
#include <QString>
#include <memory>

using aide::Application;

// NOLINTNEXTLINE
Application::Application(int argc, char* argv[])
    : QApplication(argc, argv)
{
    if (!isOrganizationNameSet()) {
        throw std::runtime_error(
            "Application name and organization name need to be set before "
            "creating an aide::Application. This ensures that the logger uses "
            "a meaningful file path and that settings are stored in a "
            "meaningful location.");
    }

    setupLogger();
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

    if (success)
    {
        FileName logPath(logLocation.append(organizationName()).append(applicationName()).append(".log").toStdString().c_str());
        LoggerName loggerName(applicationName().toStdString().c_str());
        m_logger = std::make_unique<aide::Logger>(logPath, loggerName);
    } else {
        m_logger = std::make_unique<Logger>();
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
