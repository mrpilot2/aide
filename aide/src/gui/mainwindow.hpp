
#ifndef AIDE_MAINWINDOW_HPP
#define AIDE_MAINWINDOW_HPP

#include <memory>

#include <aide/actionregistryinterface.hpp>
#include <aide/applicationconfig.hpp>

#include "applicationclose.hpp"
#include "loggerinterface.hpp"
#include "mainwindowcontroller.hpp"
#include "mainwindowinterface.hpp"

namespace Ui
{
    class MainWindow;
} // namespace Ui

class QIcon;
class QMenu;
class QWidget;

namespace aide::gui
{
    class TranslatorInterface;

    class MainWindow
        : public core::MainWindowInterface
        , public core::ApplicationCloseView
    {
        Q_OBJECT
    public:
        MainWindow(LoggerPtr loggerInterface, ApplicationConfig config,
                   QWidget* parent);
        ~MainWindow() override;
        MainWindow(const MainWindow&)             = delete;
        MainWindow& operator=(const MainWindow&)  = delete;
        MainWindow(const MainWindow&&)            = delete;
        MainWindow& operator=(const MainWindow&&) = delete;

        void setMainWindowController(
            MainWindowControllerPtr controller,
            const ActionRegistryInterfacePtr& actionRegistry);

        void restoreGeometryAndState(QByteArray geometry,
                                     QByteArray state) override;

        [[nodiscard]] std::shared_ptr<TranslatorInterface> translator() const;
        std::tuple<aide::core::UserSelection, bool>
        letUserConfirmApplicationClose() override;

    public slots:
        void refreshIcons();

    protected:
        void changeEvent(QEvent* event) override;

    private:
        void closeEvent(QCloseEvent* event) override;

        void registerActions(const ActionRegistryInterfacePtr& actionRegistry);

        void registerViewMenu(const ActionRegistryInterfacePtr& actionRegistry);

        void registerShowLogInFileManagerAction(
            QMenu* menuHelp, const ActionRegistryInterfacePtr& actionRegistry);

        void toggleFullScreen();

        [[nodiscard]] static QIcon createIconFromTheme(
            const std::string& iconName);

        LoggerPtr logger;

        ApplicationConfig m_config;

        MainWindowControllerPtr m_controller;

        std::shared_ptr<TranslatorInterface> m_translator;
        std::unique_ptr<Ui::MainWindow> m_ui;
        std::shared_ptr<QAction> m_actionSettings;
        std::shared_ptr<QAction> m_actionQuit;
        std::shared_ptr<QAction> m_actionFullScreen;
        std::shared_ptr<QAction> m_actionShowLogInFileManager;
        std::shared_ptr<QAction> m_actionAboutAide;
        std::shared_ptr<QAction> m_actionAboutQt;
    };

} // namespace aide::gui

#endif // AIDE_MAINWINDOW_HPP
