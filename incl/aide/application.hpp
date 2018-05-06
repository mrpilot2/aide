
#pragma once

#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>

namespace aide
{
    class Application
    {
    public:
        Application(int argc, char* argv[]);

        const QMainWindow* mainWindow() const;

    private:
        QApplication m_application;

        QMainWindow m_mainWindow;
    };
}
