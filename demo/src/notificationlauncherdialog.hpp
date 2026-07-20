#ifndef DEMO_NOTIFICATION_LAUNCHER_DIALOG_HPP
#define DEMO_NOTIFICATION_LAUNCHER_DIALOG_HPP

#include <vector>

#include <QDialog>

#include <aide/hierarchicalid.hpp>
#include <aide/notificationtype.hpp>

class QComboBox;
class QPushButton;
class QWidget;

namespace aide
{
    class NotificationManagerInterface;
} // namespace aide

namespace aide::core
{
    class MainWindowInterface;
} // namespace aide::core

namespace aide::widgets
{
    class Banner;
} // namespace aide::widgets

namespace demo
{
    // Shell opened by the demo's "Demo -> Notifications" menu action (#164).
    // Owns the "Balloon", "Got it" (#165), "Dialog banner" (#166) and
    // "Editor banner" (#167) sections.
    class NotificationLauncherDialog : public QDialog
    {
        Q_OBJECT
    public:
        NotificationLauncherDialog(
            aide::NotificationManagerInterface& notificationManager,
            aide::core::MainWindowInterface& mainWindow,
            aide::HierarchicalId balloonGroupId,
            aide::HierarchicalId stickyBalloonGroupId,
            QWidget* parent = nullptr);

    private:
        [[nodiscard]] QWidget* createBalloonSection();
        [[nodiscard]] QWidget* createGotItSection();
        [[nodiscard]] QWidget* createDialogBannerSection();
        [[nodiscard]] QWidget* createEditorBannerSection();

        void postBalloon(aide::NotificationType type, const QString& title,
                         const QString& content);
        void postBalloonWithActions();
        void postStickyBalloon();

        void showGotIt();
        static void resetGotItSeenFlag();

        void addEditorBanner(aide::NotificationType type,
                             const QString& message);
        void clearEditorBanners();

        aide::NotificationManagerInterface& m_notificationManager;
        aide::core::MainWindowInterface& m_mainWindow;
        aide::HierarchicalId m_balloonGroupId;
        aide::HierarchicalId m_stickyBalloonGroupId;

        QPushButton* m_gotItAnchorButton{nullptr};
        QComboBox* m_gotItPositionCombo{nullptr};

        std::vector<aide::widgets::Banner*> m_editorBanners;
    };
} // namespace demo

#endif // DEMO_NOTIFICATION_LAUNCHER_DIALOG_HPP
