#ifndef AIDE_NOTIFICATION_DISPLAY_TYPE_HPP
#define AIDE_NOTIFICATION_DISPLAY_TYPE_HPP

namespace aide
{
    // Group-routed popup destinations only. Editor banner, dialog banner and
    // "Got it" are caller-driven surfaces and deliberately have no enumerator
    // here (see issue #145).
    enum class NotificationDisplayType
    {
        None,
        Balloon,
        StickyBalloon
    };
} // namespace aide

#endif // AIDE_NOTIFICATION_DISPLAY_TYPE_HPP
