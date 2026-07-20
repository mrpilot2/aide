#ifndef DEMO_NOTIFICATION_LAUNCHER_DIALOG_HPP
#define DEMO_NOTIFICATION_LAUNCHER_DIALOG_HPP

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

namespace demo
{
    // Shell opened by the demo's "Demo -> Notifications" menu action (#164).
    // Owns the "Balloon", "Got it" (#165) and "Dialog banner" (#166)
    // sections; the editor banner (#167) section attaches its own QGroupBox
    // here as that ticket lands.
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
        [[nodiscard]] QWidget* createGotItSection();
        [[nodiscard]] QWidget* createDialogBannerSection();

        void postBalloon(aide::NotificationType type, const QString& title,
                         const QString& content);
        void postBalloonWithActions();
        void postStickyBalloon();

        void showGotIt();
        static void resetGotItSeenFlag();

        aide::NotificationManagerInterface& m_notificationManager;
        aide::HierarchicalId m_balloonGroupId;
        aide::HierarchicalId m_stickyBalloonGroupId;

        QPushButton* m_gotItAnchorButton{nullptr};
        QComboBox* m_gotItPositionCombo{nullptr};
    };
} // namespace demo

#endif // DEMO_NOTIFICATION_LAUNCHER_DIALOG_HPP
