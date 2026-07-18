
#ifndef AIDE_BANNER_HPP
#define AIDE_BANNER_HPP

#include <QFrame>
#include <QString>

#include <aide/gui/widgets/aidewidgets_export.h>
#include <aide/notificationaction.hpp>
#include <aide/notificationtype.hpp>

class QHBoxLayout;
class QLabel;
class QToolButton;
class QWidget;

namespace aide::widgets
{
    /**
     * @brief Severity-tinted inline strip: coloured left stripe, type icon,
     * message, optional action links, and a trailing close button.
     *
     * Caller-driven and light-payload: reuses NotificationType /
     * NotificationAction from the core notification model but never goes
     * through NotificationManager::post() and is never logged (see #145).
     */
    class AIDEWIDGETS_EXPORT Banner : public QFrame
    {
        Q_OBJECT
    public:
        Banner(NotificationType type, const QString& message,
               QWidget* parent = nullptr);

        void addAction(const NotificationAction& action);

        void setClosable(bool closable);

    signals:
        void closed();

    private:
        QLabel* m_iconLabel{nullptr};
        QLabel* m_messageLabel{nullptr};
        QToolButton* m_closeButton{nullptr};
        QHBoxLayout* m_contentLayout{nullptr};
    };
} // namespace aide::widgets

#endif // AIDE_BANNER_HPP
