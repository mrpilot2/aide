#include "notificationballoonhost.hpp"

#include <algorithm>

#include <QGuiApplication>
#include <QScreen>
#include <QWidget>

#include "aide/gui/widgets/notificationballoon.hpp"
#include "aide/notificationdisplaytype.hpp"
#include "aide/notificationmanagerinterface.hpp"
#include "aide/settingsinterface.hpp"
#include "settings/notifications/notificationsettingskeys.hpp"

using aide::Notification;
using aide::NotificationBalloonPlacement;
using aide::NotificationId;
using aide::NotificationManagerInterface;
using aide::SettingsInterface;
using aide::gui::NotificationBalloonHost;
using aide::widgets::NotificationBalloon;

namespace
{
    constexpr int SCREEN_MARGIN{16};
    constexpr int BALLOON_SPACING{8};

    bool isRightCorner(NotificationBalloonPlacement placement)
    {
        return placement == NotificationBalloonPlacement::BottomRight ||
               placement == NotificationBalloonPlacement::TopRight;
    }

    bool isBottomCorner(NotificationBalloonPlacement placement)
    {
        return placement == NotificationBalloonPlacement::BottomRight ||
               placement == NotificationBalloonPlacement::BottomLeft;
    }
} // namespace

NotificationBalloonHost::NotificationBalloonHost(
    NotificationManagerInterface& notificationManager,
    SettingsInterface& settings, QWidget* anchorWidget, QObject* parent)
    : QObject(parent)
    , m_notificationManager(notificationManager)
    , m_settings(settings)
    , m_anchorWidget(anchorWidget)
{}

void NotificationBalloonHost::onNotificationPosted(NotificationId id)
{
    const auto displayType = m_notificationManager.resolvedDisplayType(id);
    if (displayType == aide::NotificationDisplayType::None) { return; }

    const auto notification = m_notificationManager.notification(id);
    if (!notification.has_value()) { return; }

    showBalloon(*notification,
                displayType == aide::NotificationDisplayType::StickyBalloon);
}

NotificationBalloonPlacement NotificationBalloonHost::placement() const
{
    const auto value = m_settings.value(
        core::notificationBalloonPlacementKey(),
        static_cast<int>(NotificationBalloonPlacement::BottomRight));
    return static_cast<NotificationBalloonPlacement>(value.toInt());
}

QRect NotificationBalloonHost::anchorScreenGeometry() const
{
    if (m_anchorWidget != nullptr) {
        if (auto* screen = m_anchorWidget->screen(); screen != nullptr) {
            return screen->availableGeometry();
        }
    }
    if (auto* screen = QGuiApplication::primaryScreen(); screen != nullptr) {
        return screen->availableGeometry();
    }
    return {};
}

void NotificationBalloonHost::showBalloon(const Notification& notification,
                                          bool sticky)
{
    auto* balloon = new NotificationBalloon(
        notification.type, notification.title, notification.content, sticky);

    for (const auto& action : notification.actions) {
        balloon->addAction(action);
    }

    connect(balloon, &NotificationBalloon::closed, this,
            [this, balloon]() { removeBalloon(balloon); });

    m_balloons.push_back(balloon);
    relayout(true);
}

void NotificationBalloonHost::removeBalloon(NotificationBalloon* balloon)
{
    const auto it = std::find(m_balloons.begin(), m_balloons.end(), balloon);
    if (it == m_balloons.end()) { return; }

    m_balloons.erase(it);
    balloon->setParent(nullptr);
    balloon->deleteLater();
    relayout(false);
}

void NotificationBalloonHost::relayout(bool animateNewest)
{
    const auto screenRect       = anchorScreenGeometry();
    const auto currentPlacement = placement();
    const bool right            = isRightCorner(currentPlacement);
    const bool bottom           = isBottomCorner(currentPlacement);

    int edgeY = bottom ? screenRect.bottom() - SCREEN_MARGIN
                       : screenRect.top() + SCREEN_MARGIN;

    // Newest balloon (back of the vector) sits nearest the corner; older
    // balloons are pushed toward screen center as new ones arrive.
    for (auto it = m_balloons.rbegin(); it != m_balloons.rend(); ++it) {
        auto* balloon    = *it;
        const int width  = balloon->width();
        const int height = balloon->sizeHint().height();

        const int posX = right ? screenRect.right() - SCREEN_MARGIN - width
                               : screenRect.left() + SCREEN_MARGIN;
        const int posY = bottom ? edgeY - height : edgeY;

        const bool isNewest = animateNewest && it == m_balloons.rbegin();
        if (isNewest) {
            const QPoint targetPos(posX, posY);
            const QPoint startPos =
                bottom ? QPoint(posX, screenRect.bottom())
                       : QPoint(posX, screenRect.top() - height);
            balloon->resize(width, height);
            balloon->slideIn(startPos, targetPos);
        } else {
            balloon->setGeometry(posX, posY, width, height);
        }

        edgeY =
            bottom ? posY - BALLOON_SPACING : posY + height + BALLOON_SPACING;
    }
}
