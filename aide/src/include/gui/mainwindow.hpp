
#ifndef AIDE_MAINWINDOW_HPP
#define AIDE_MAINWINDOW_HPP

#include <memory>

#include <QMainWindow>

namespace Ui
{
    class MainWindow;
} // namespace Ui

class QWidget;

namespace aide
{
    namespace gui
    {
        class MainWindow : public QMainWindow
        {
        public:
            explicit MainWindow(QWidget* parent = nullptr);
            ~MainWindow() override;
            MainWindow(const MainWindow&) = delete;
            MainWindow& operator=(const MainWindow&) = delete;
            MainWindow(const MainWindow&&)           = delete;
            MainWindow& operator=(const MainWindow&&) = delete;

        private:
            std::unique_ptr<Ui::MainWindow> m_ui;
        };
    } // namespace gui

} // namespace aide

#endif // AIDE_MAINWINDOW_HPP
