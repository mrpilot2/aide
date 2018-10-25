
#pragma once

#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>

namespace aide
{
    class Application
    {
    public:
        Application() = delete;
        Application(int& argc, char** argv);

        QMainWindow* mainWindow() const;

        static int exec();

    private:
        QApplication m_application;

        QMainWindow* m_mainWindow;
    };
}  // namespace aide
