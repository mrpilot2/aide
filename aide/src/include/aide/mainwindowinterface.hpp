#ifndef AIDE_MAIN_WINDOW_INTERFACE_HPP
#define AIDE_MAIN_WINDOW_INTERFACE_HPP

#include <memory>

#include <QMainWindow>

#include <aide/notificationtype.hpp>

class QString;

namespace aide::widgets
{
    class Banner;
    class NotificationView;
} // namespace aide::widgets

namespace aide::core
{
    class MainWindowInterface : public QMainWindow
    {
    public:
        explicit MainWindowInterface(QWidget* parent = nullptr);

        virtual ~MainWindowInterface();

        virtual void restoreGeometryAndState(QByteArray geometry,
                                             QByteArray state) = 0;

        virtual aide::widgets::Banner* addBanner(NotificationType type,
                                                 const QString& message) = 0;

        virtual void removeBanner(aide::widgets::Banner* banner) = 0;

        /**
         * @brief The aIDE-owned notification log view (#155), already wired
         * so its "Notification Settings..." overflow entries open the
         * Settings dialog on the built-in Notifications page. Not added to
         * any layout by aIDE itself -- the consumer embeds it wherever they
         * want (e.g. a side panel).
         */
        [[nodiscard]] virtual aide::widgets::NotificationView*
        notificationLogView() const = 0;
    };

    using MainWindowInterfaceWeakPtr = std::weak_ptr<MainWindowInterface>;
} // namespace aide::core

#endif // AIDE_MAIN_WINDOW_INTERFACE_HPP
