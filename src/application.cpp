

#include <application.hpp>

#include "application.hpp"

using aide::Application;

Application::Application(int &argc, char **argv)
    : m_application{argc, argv}
    , m_mainWindow{new QMainWindow}
{
}

QMainWindow* Application::mainWindow() const
{
    return m_mainWindow;
}

int Application::exec()
{
   return QApplication::exec();
}
