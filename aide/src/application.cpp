
#include "application.hpp"

#include <QTimer>

#include "aidesettingsprovider.hpp"
#include "applicationbuilder.hpp"
#include "gui/mainwindow.hpp"
#include "logger.hpp"
#include "qtsettings.hpp"

using aide::Application;
using aide::ApplicationBuilder;
using aide::LoggerPtr;
using aide::gui::TranslatorInterface;

// NOLINTNEXTLINE
Application::Application(int& argc, char* argv[])
    : QApplication(argc, argv)
{
    AideSettingsProvider::provideVersionableSettings(
        std::make_shared<QtSettings>(true));
    AideSettingsProvider::provideUnVersionableSettings(
        std::make_shared<QtSettings>(false));
    m_appBuilder = std::make_shared<ApplicationBuilder>();

    if (!isOrganizationNameSet()) {
        throw std::runtime_error(
            "Application name and organization name need to be set before "
            "creating an aide::Application. This ensures that the logger uses "
            "a meaningful file path and that settings are stored in a "
            "meaningful location.");
    }

    constexpr int delayedSetupTimeInMs(1000);
    // NOLINTNEXTLINE
    auto* timer = new QTimer(this);
    timer->setSingleShot(true);
    connect(timer, &QTimer::timeout, [=]() {
        QApplication::setApplicationDisplayName("");
        timer->deleteLater();
    });
    timer->start(delayedSetupTimeInMs);
}

LoggerPtr Application::logger()
{
    return logger(QApplication::applicationName().toStdString());
}

LoggerPtr Application::logger(const std::string& loggerName)
{
    return ApplicationBuilder::setupLogger(loggerName);
}

std::shared_ptr<QMainWindow> Application::mainWindow() const
{
    return m_appBuilder->mainWindow();
}

bool Application::isOrganizationNameSet()
{
    return !organizationName().isEmpty();
}

std::shared_ptr<TranslatorInterface> Application::translator() const
{
    return m_appBuilder->mainWindow()->translator();
}

std::shared_ptr<aide::AideSettingsProvider> Application::settingsProvider()
    const
{
    return m_appBuilder->settingsProvider();
}

aide::ActionRegistryInterfacePtr Application::actionRegistry() const
{
    return m_appBuilder->actionRegistry();
}
