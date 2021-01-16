
#ifndef AIDE_MAINWINDOW_HPP
#define AIDE_MAINWINDOW_HPP

#include <memory>

#include <QMainWindow>

#include <aide/actionregistryinterface.hpp>

#include "applicationclose.hpp"
#include "loggerinterface.hpp"
#include "mainwindowcontroller.hpp"
#include "mainwindowinterface.hpp"

namespace Ui
{
    class MainWindow;
} // namespace Ui

class QIcon;
class QWidget;

namespace aide
{
    namespace gui
    {
        class TranslatorInterface;

        class MainWindow
            : public aide::core::MainWindowInterface
            , public aide::core::ApplicationCloseView
        {
            Q_OBJECT
        public:
            explicit MainWindow(LoggerPtr loggerInterface, QWidget* parent);
            ~MainWindow() override;
            MainWindow(const MainWindow&) = delete;
            MainWindow& operator=(const MainWindow&) = delete;
            MainWindow(const MainWindow&&)           = delete;
            MainWindow& operator=(const MainWindow&&) = delete;

            void setMainWindowController(
                MainWindowControllerPtr controller,
                const ActionRegistryInterfacePtr& actionRegistry);

            void restoreGeometryAndState(QByteArray geometry,
                                         QByteArray state) override;

            [[nodiscard]] std::shared_ptr<TranslatorInterface> translator()
                const;
            std::tuple<aide::core::UserSelection, bool>
            letUserConfirmApplicationClose() override;

        private:
            void closeEvent(QCloseEvent* event) override;

            void registerActions(
                const ActionRegistryInterfacePtr& actionRegistry);

            [[nodiscard]] static QIcon createIconFromTheme(
                const std::string& iconName);

            LoggerPtr logger;

            MainWindowControllerPtr m_controller;

            std::shared_ptr<TranslatorInterface> m_translator;
            std::unique_ptr<Ui::MainWindow> m_ui;
            std::shared_ptr<QAction> m_actionSettings;
            std::shared_ptr<QAction> m_actionQuit;
            std::shared_ptr<QAction> m_actionAboutQt;
        };

    } // namespace gui
} // namespace aide
#endif // AIDE_MAINWINDOW_HPP
