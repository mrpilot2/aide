
#include "application.hpp"

#include <actionregistry.hpp>
#include <utility>

#include <QDir>
#include <QStandardPaths>
#include <QString>

#include <gui/mainwindow.hpp>

using aide::Application;
using aide::Logger;
using aide::gui::MainWindow;
using aide::gui::TranslatorInterface;

// NOLINTNEXTLINE
Application::Application(int& argc, char* argv[])
    : QApplication(argc, argv)
    , m_actionRegistry{std::make_shared<ActionRegistry>()}
    , m_mainWindow(new MainWindow(m_actionRegistry, nullptr))
{
    if (!isOrganizationNameSet()) {
        throw std::runtime_error(
            "Application name and organization name need to be set before "
            "creating an aide::Application. This ensures that the logger uses "
            "a meaningful file path and that settings are stored in a "
            "meaningful location.");
    }

    m_mainWindow->showMaximized();
}

std::shared_ptr<Logger> aide::Application::logger() const
{
    return m_logger;
}

std::shared_ptr<QMainWindow> aide::Application::mainWindow() const
{
    return m_mainWindow;
}

bool Application::isOrganizationNameSet()
{
    return !organizationName().isEmpty();
}

std::shared_ptr<Logger> Application::setupLogger()
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

        return std::make_shared<aide::Logger>(logPath);
    }
    return std::make_shared<Logger>();
}

bool Application::tryToCreateLogLocationIfItDoesNotExist(
    const QString& logLocation)
{
    if (!logLocation.isEmpty()) {
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
    return false;
}

std::shared_ptr<TranslatorInterface> Application::translator() const
{
    return m_mainWindow->translator();
}
