
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

        auto mainWindow() const -> QMainWindow*;

        static auto exec() -> int;

    private:
        QApplication m_application;

        QMainWindow* m_mainWindow;
    };
}  // namespace aide
