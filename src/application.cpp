

#include "application.hpp"

using aide::Application;

Application::Application(int argc, char** argv)
    : m_application{argc, argv}
{
}

const QMainWindow* Application::mainWindow() const
{
    return &m_mainWindow;
}
