
#include "application.hpp"

#include <QString>

using aide::Application;

// NOLINTNEXTLINE
Application::Application(int argc, char* argv[]) :
    QApplication(argc, argv)
{
    if (!isOrganizationNameSet())
    {
        throw std::runtime_error(
            "Application name and organization name need to be set before "
            "creating an aide::Application. This ensures that the logger uses "
            "a meaningful file path and that settings are stored in a "
            "meaningful location.");
    }
}
bool Application::isOrganizationNameSet()
{
    return !organizationName().isEmpty();
}
