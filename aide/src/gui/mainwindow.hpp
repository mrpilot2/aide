
#ifndef AIDE_MAINWINDOW_HPP
#define AIDE_MAINWINDOW_HPP

#include <memory>
#include <vector>

#include <aide/actionregistryinterface.hpp>
#include <aide/applicationconfig.hpp>
#include <aide/mainwindowinterface.hpp>
#include <aide/notificationtype.hpp>

#include "applicationclose.hpp"
#include "loggerinterface.hpp"
#include "mainwindowcontroller.hpp"

namespace Ui
{
    class MainWindow;
} // namespace Ui

namespace aide::widgets
{
    class Banner;
    class NotificationView;
} // namespace aide::widgets

namespace aide
{
    class NotificationManagerInterface;
    class SettingsInterface;
} // namespace aide

class QAction;
class QIcon;
class QMenu;
class QString;
class QVBoxLayout;
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

        /**
         * @brief Push an editor-mount banner above the central widget and
         * return it. Wraps whatever is currently the central widget on the
         * first call; several active banners stack vertically, newest at
         * the bottom, with a hairline divider between them. Caller-driven:
         * never goes through NotificationManager::post() (see #145).
         */
        aide::widgets::Banner* addBanner(NotificationType type,
                                         const QString& message) override;

        /**
         * @brief Remove and delete a banner previously returned by
         * addBanner(). No-op if @p banner is not currently mounted.
         */
        void removeBanner(aide::widgets::Banner* banner) override;

        /**
         * @brief Creates the notification log view (#155), wiring its
         * settingsRequested()/groupSettingsRequested() signals to open
         * Settings on the built-in Notifications page. Called once by
         * ApplicationBuilder, after setMainWindowController(); the returned
         * pointer (via notificationLogView()) is not added to any layout.
         */
        void createNotificationLogView(
            aide::NotificationManagerInterface& manager,
            aide::SettingsInterface& settings);

        [[nodiscard]] aide::widgets::NotificationView* notificationLogView()
            const override;

    public slots:
        void refreshIcons();

    protected:
        void changeEvent(QEvent* event) override;

    private:
        void closeEvent(QCloseEvent* event) override;

        void registerActions(const ActionRegistryInterfacePtr& actionRegistry);

        void registerViewMenu(const ActionRegistryInterfacePtr& actionRegistry);

        void registerGatedHelpActions(
            QMenu* menuHelp, const ActionRegistryInterfacePtr& actionRegistry);

        void toggleFullScreen();

        [[nodiscard]] static QIcon createIconFromTheme(
            const std::string& iconName);

        void ensureBannerHost();
        void rebuildBannerHostLayout();

        LoggerPtr logger;

        ApplicationConfig m_config;

        MainWindowControllerPtr m_controller;

        std::shared_ptr<TranslatorInterface> m_translator;
        std::unique_ptr<Ui::MainWindow> m_ui;
        QAction* m_actionSettings{nullptr};
        QAction* m_actionQuit{nullptr};
        QAction* m_actionFullScreen{nullptr};
        QAction* m_actionShowLogInFileManager{nullptr};
        QAction* m_actionReportBug{nullptr};
        QAction* m_actionAboutAide{nullptr};
        QAction* m_actionAboutQt{nullptr};

        QWidget* m_bannerWrapper{nullptr};
        QWidget* m_bannerHost{nullptr};
        QVBoxLayout* m_bannerHostLayout{nullptr};
        std::vector<aide::widgets::Banner*> m_banners;

        aide::widgets::NotificationView* m_notificationLogView{nullptr};
    };

} // namespace aide::gui

#endif // AIDE_MAINWINDOW_HPP
