#ifndef DEMO_NOTIFICATION_DEMO_DIALOG_HPP
#define DEMO_NOTIFICATION_DEMO_DIALOG_HPP

#include <aide/gui/widgets/aidedialog.hpp>

class QWidget;

namespace demo
{
    // Opened by NotificationLauncherDialog's "Dialog banner" section (#166):
    // demonstrates AideDialog's shared banner slot (showBanner()/
    // clearBanner(), see #146) directly, without going through
    // NotificationManager.
    class NotificationDemoDialog : public aide::widgets::AideDialog
    {
        Q_OBJECT
    public:
        explicit NotificationDemoDialog(QWidget* parent = nullptr);
    };
} // namespace demo

#endif // DEMO_NOTIFICATION_DEMO_DIALOG_HPP
