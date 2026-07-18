#ifndef AIDE_NOTIFICATION_ACTION_HPP
#define AIDE_NOTIFICATION_ACTION_HPP

#include <functional>

#include <QString>

namespace aide
{
    struct NotificationAction
    {
        QString title;
        std::function<void()> handler;
    };
} // namespace aide

#endif // AIDE_NOTIFICATION_ACTION_HPP
