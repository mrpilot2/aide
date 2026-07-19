#ifndef DEMO_NOTIFICATION_LAUNCHER_DIALOG_HPP
#define DEMO_NOTIFICATION_LAUNCHER_DIALOG_HPP

#include <QDialog>

#include <aide/hierarchicalid.hpp>
#include <aide/notificationtype.hpp>

class QWidget;

namespace aide
{
    class NotificationManagerInterface;
} // namespace aide

namespace demo
{
    // Shell opened by the demo's "Demo -> Notifications" menu action (#164).
    // Owns only the "Balloon" section; the "Got it" (#165), dialog banner
    // (#166) and editor banner (#167) sections attach their own QGroupBox
    // here as those tickets land.
    class NotificationLauncherDialog : public QDialog
    {
        Q_OBJECT
    public:
        NotificationLauncherDialog(
            aide::NotificationManagerInterface& notificationManager,
            aide::HierarchicalId balloonGroupId,
            aide::HierarchicalId stickyBalloonGroupId,
            QWidget* parent = nullptr);

    private:
        [[nodiscard]] QWidget* createBalloonSection();

        void postBalloon(aide::NotificationType type, const QString& title,
                         const QString& content);
        void postBalloonWithActions();
        void postStickyBalloon();

        aide::NotificationManagerInterface& m_notificationManager;
        aide::HierarchicalId m_balloonGroupId;
        aide::HierarchicalId m_stickyBalloonGroupId;
    };
} // namespace demo

#endif // DEMO_NOTIFICATION_LAUNCHER_DIALOG_HPP
