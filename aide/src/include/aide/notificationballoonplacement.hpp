#ifndef AIDE_NOTIFICATION_BALLOON_PLACEMENT_HPP
#define AIDE_NOTIFICATION_BALLOON_PLACEMENT_HPP

namespace aide
{
    // Global corner anchor for every balloon (one setting, not per-group).
    // Stacking direction derives from the corner alone: balloons anchor here
    // and grow toward screen center as more stack up (see #147/#152).
    enum class NotificationBalloonPlacement
    {
        BottomRight,
        BottomLeft,
        TopRight,
        TopLeft
    };
} // namespace aide

#endif // AIDE_NOTIFICATION_BALLOON_PLACEMENT_HPP
