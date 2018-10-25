

#include <application.hpp>

#include "application.hpp"

using aide::Application;

Application::Application(int &argc, char **argv)
    : m_application{argc, argv}
    , m_mainWindow{new QMainWindow}
{
}

auto Application::mainWindow() const -> QMainWindow*
{
    return m_mainWindow;
}

auto Application::exec() -> int
{
   return QApplication::exec();
}
