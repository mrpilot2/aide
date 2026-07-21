#ifndef AIDE_NOTIFICATION_BALLOON_HPP
#define AIDE_NOTIFICATION_BALLOON_HPP

#include <QPoint>
#include <QString>
#include <QWidget>

#include <aide/gui/widgets/aidewidgets_export.h>
#include <aide/notificationaction.hpp>
#include <aide/notificationtype.hpp>

class QEnterEvent;
class QHBoxLayout;
class QLabel;
class QMenu;
class QProgressBar;
class QPropertyAnimation;
class QTimer;
class QToolButton;

namespace aide::widgets
{
    /**
     * @brief Frameless corner popup: the one post()-driven, logged display
     * surface (see #145). Severity-tinted left stripe + type icon + title +
     * body + up to two inline action links, the rest under a "More" menu,
     * plus a close button. Mirrors Banner's look (#146) as a floating popup
     * rather than an inline strip.
     *
     * Transient balloons auto-dismiss after a fixed delay with a countdown
     * progress bar that pauses while the mouse hovers the balloon; sticky
     * balloons never time out and are closed by the user only. Created and
     * owned exclusively by NotificationBalloonHost.
     */
    class AIDEWIDGETS_EXPORT NotificationBalloon : public QWidget
    {
        Q_OBJECT
    public:
        NotificationBalloon(NotificationType type, const QString& title,
                            const QString& content, bool sticky,
                            QWidget* parent = nullptr);

        void addAction(const NotificationAction& action);

        /**
         * @brief Show the balloon and slide it from @p fromPos to @p toPos
         * (both top-left points in the parent widget's local coordinate
         * space).
         */
        void slideIn(const QPoint& fromPos, const QPoint& toPos);

    signals:
        void closed();

    protected:
        void enterEvent(QEnterEvent* event) override;
        void leaveEvent(QEvent* event) override;

    private:
        void onCountdownTick();

        QLabel* m_iconLabel{nullptr};
        QLabel* m_titleLabel{nullptr};
        QLabel* m_contentLabel{nullptr};
        QToolButton* m_closeButton{nullptr};
        QToolButton* m_moreButton{nullptr};
        QMenu* m_moreMenu{nullptr};
        QHBoxLayout* m_actionsLayout{nullptr};
        QProgressBar* m_countdownBar{nullptr};
        QPropertyAnimation* m_slideAnimation{nullptr};
        QTimer* m_countdownTimer{nullptr};

        int m_inlineActionCount{0};
        int m_remainingMs{0};
    };
} // namespace aide::widgets

#endif // AIDE_NOTIFICATION_BALLOON_HPP
