#ifndef AIDE_NOTIFICATION_BALLOON_HOST_HPP
#define AIDE_NOTIFICATION_BALLOON_HOST_HPP

#include <vector>

#include <QObject>

#include <aide/notification.hpp>
#include <aide/notificationballoonplacement.hpp>
#include <aide/notificationid.hpp>

namespace aide
{
    class NotificationManagerInterface;
    class SettingsInterface;
} // namespace aide

class QRect;
class QWidget;

namespace aide::widgets
{
    class NotificationBalloon;
} // namespace aide::widgets

class QEvent;

namespace aide::gui
{
    /**
     * @brief Observes NotificationManagerInterface::notificationPosted (via
     * the concrete NotificationManager signal, wired externally — see #145)
     * and shows a NotificationBalloon for every notification whose frozen
     * resolvedDisplayType is Balloon or StickyBalloon; None is log-only and
     * never reaches here.
     *
     * Corner placement is a single global setting
     * (notifications.balloonPlacement, #147); stacking direction derives
     * from that corner alone, growing toward screen center as balloons
     * accumulate. No GUI dependency leaks into core: this class lives in
     * aIDE's GUI layer and talks to the core manager purely through
     * NotificationManagerInterface.
     */
    class NotificationBalloonHost : public QObject
    {
        Q_OBJECT
    public:
        NotificationBalloonHost(
            aide::NotificationManagerInterface& notificationManager,
            aide::SettingsInterface& settings, QWidget* anchorWidget,
            QObject* parent = nullptr);

    protected:
        bool eventFilter(QObject* watched, QEvent* event) override;

    private:
        [[nodiscard]] aide::NotificationBalloonPlacement placement() const;
        [[nodiscard]] QRect anchorRect() const;

        void showBalloon(const aide::Notification& notification, bool sticky);
        void removeBalloon(aide::widgets::NotificationBalloon* balloon);
        void relayout(bool animateNewest);
        void updateBalloonVisibilityForWindowState();

        aide::NotificationManagerInterface& m_notificationManager;
        aide::SettingsInterface& m_settings;
        QWidget* m_anchorWidget;

        std::vector<aide::widgets::NotificationBalloon*> m_balloons;

    private slots:
        void onNotificationPosted(aide::NotificationId id);
    };
} // namespace aide::gui

#endif // AIDE_NOTIFICATION_BALLOON_HOST_HPP
