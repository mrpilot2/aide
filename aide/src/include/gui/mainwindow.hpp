
#ifndef AIDE_MAINWINDOW_HPP
#define AIDE_MAINWINDOW_HPP

#include <memory>

#include <QMainWindow>
#include <QtCore/QTranslator>

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
            QTranslator qtTranslator;
            QTranslator translator;

            std::unique_ptr<Ui::MainWindow> m_ui;
        };
    } // namespace gui

} // namespace aide

#endif // AIDE_MAINWINDOW_HPP
