
#include "application.hpp"

#include <utility>

#include <QTimer>

#include "applicationbuilder.hpp"
#include "core/actionregistry.hpp"
#include "gui/mainwindow.hpp"

using aide::Application;
using aide::ApplicationBuilder;
using aide::Logger;
using aide::gui::TranslatorInterface;

// NOLINTNEXTLINE
Application::Application(int& argc, char* argv[])
    : QApplication(argc, argv)
    , m_appBuilder(std::make_shared<ApplicationBuilder>())
{
    if (!isOrganizationNameSet()) {
        throw std::runtime_error(
            "Application name and organization name need to be set before "
            "creating an aide::Application. This ensures that the logger uses "
            "a meaningful file path and that settings are stored in a "
            "meaningful location.");
    }

    m_appBuilder->mainWindow()->showMaximized();

    const int delayedSetupTimeInMs(1000);
    // NOLINTNEXTLINE
    auto* timer = new QTimer();
    timer->setSingleShot(true);
    connect(timer, &QTimer::timeout, [=]() {
        QApplication::setApplicationDisplayName("");
        timer->deleteLater();
    });
    timer->start(delayedSetupTimeInMs);
}

std::shared_ptr<Logger> aide::Application::logger() const
{
    return m_appBuilder->logger();
}

std::shared_ptr<QMainWindow> aide::Application::mainWindow() const
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
