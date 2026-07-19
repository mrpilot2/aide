#include "notificationballoon.hpp"

#include <QEnterEvent>
#include <QEvent>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QMenu>
#include <QProgressBar>
#include <QPropertyAnimation>
#include <QTimer>
#include <QToolButton>
#include <QVBoxLayout>

using aide::NotificationAction;
using aide::NotificationType;
using aide::widgets::NotificationBalloon;

namespace
{
    constexpr int BALLOON_WIDTH{320};
    constexpr int STRIPE_WIDTH{4};
    constexpr int CONTENT_MARGIN{10};
    constexpr int LAYOUT_SPACING{6};
    constexpr int ICON_SIZE{18};
    constexpr int MAX_INLINE_ACTIONS{2};

    constexpr int DISMISS_MS{8000};
    constexpr int COUNTDOWN_TICK_MS{100};
    constexpr int SLIDE_ANIMATION_MS{200};

    constexpr int INFORMATION_STRIPE_R{25};
    constexpr int INFORMATION_STRIPE_G{118};
    constexpr int INFORMATION_STRIPE_B{210};
    constexpr int INFORMATION_BACKGROUND_R{227};
    constexpr int INFORMATION_BACKGROUND_G{242};
    constexpr int INFORMATION_BACKGROUND_B{253};

    constexpr int SUCCESS_STRIPE_R{56};
    constexpr int SUCCESS_STRIPE_G{142};
    constexpr int SUCCESS_STRIPE_B{60};
    constexpr int SUCCESS_BACKGROUND_R{232};
    constexpr int SUCCESS_BACKGROUND_G{245};
    constexpr int SUCCESS_BACKGROUND_B{233};

    constexpr int WARNING_STRIPE_R{245};
    constexpr int WARNING_STRIPE_G{124};
    constexpr int WARNING_STRIPE_B{0};
    constexpr int WARNING_BACKGROUND_R{255};
    constexpr int WARNING_BACKGROUND_G{243};
    constexpr int WARNING_BACKGROUND_B{224};

    constexpr int ERROR_STRIPE_R{211};
    constexpr int ERROR_STRIPE_G{47};
    constexpr int ERROR_STRIPE_B{47};
    constexpr int ERROR_BACKGROUND_R{255};
    constexpr int ERROR_BACKGROUND_G{235};
    constexpr int ERROR_BACKGROUND_B{238};

    struct BalloonPalette
    {
        QColor stripeColor;
        QColor backgroundColor;
        QString iconThemeName;
    };

    // Placeholder palette/icon-name table, duplicated from Banner's own
    // placeholder: both surfaces switch to AppearanceManager::severityColor()
    // once #157 lands per-type assets.
    BalloonPalette paletteFor(NotificationType type)
    {
        switch (type) {
        case NotificationType::Success:
            return {
                QColor(SUCCESS_STRIPE_R, SUCCESS_STRIPE_G, SUCCESS_STRIPE_B),
                QColor(SUCCESS_BACKGROUND_R, SUCCESS_BACKGROUND_G,
                       SUCCESS_BACKGROUND_B),
                "emblem-ok"};
        case NotificationType::Warning:
            return {
                QColor(WARNING_STRIPE_R, WARNING_STRIPE_G, WARNING_STRIPE_B),
                QColor(WARNING_BACKGROUND_R, WARNING_BACKGROUND_G,
                       WARNING_BACKGROUND_B),
                "dialog-warning"};
        case NotificationType::Error:
            return {QColor(ERROR_STRIPE_R, ERROR_STRIPE_G, ERROR_STRIPE_B),
                    QColor(ERROR_BACKGROUND_R, ERROR_BACKGROUND_G,
                           ERROR_BACKGROUND_B),
                    "dialog-error"};
        case NotificationType::Information:
        default:
            return {QColor(INFORMATION_STRIPE_R, INFORMATION_STRIPE_G,
                           INFORMATION_STRIPE_B),
                    QColor(INFORMATION_BACKGROUND_R, INFORMATION_BACKGROUND_G,
                           INFORMATION_BACKGROUND_B),
                    "dialog-information"};
        }
    }

    QIcon iconFromTheme(const QString& name)
    {
        return QIcon::hasThemeIcon(name) ? QIcon::fromTheme(name) : QIcon();
    }
} // namespace

NotificationBalloon::NotificationBalloon(NotificationType type,
                                         const QString& title,
                                         const QString& content, bool sticky,
                                         QWidget* parent)
    : QWidget(parent)
    , m_iconLabel(new QLabel(this))
    , m_titleLabel(new QLabel(title, this))
    , m_contentLabel(new QLabel(content, this))
    , m_closeButton(new QToolButton(this))
    , m_actionsLayout(new QHBoxLayout)
{
    // No WindowStaysOnTopHint: that hint floats above every window on the
    // desktop, not just this app's own window, so switching to another app
    // (Alt+Tab) left the balloon stranded on top of it. Qt::Tool with a
    // parent instead makes this a transient child of the parent's window,
    // so the window manager keeps it above only that window and moves,
    // raises, and minimizes it together with its owner.
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);
    setFixedWidth(BALLOON_WIDTH);

    const auto palette = paletteFor(type);

    auto* stripe = new QWidget(this);
    stripe->setFixedWidth(STRIPE_WIDTH);
    stripe->setAutoFillBackground(true);
    QPalette stripePalette = stripe->palette();
    stripePalette.setColor(QPalette::Window, palette.stripeColor);
    stripe->setPalette(stripePalette);

    auto* panel = new QWidget(this);
    panel->setAutoFillBackground(true);
    QPalette panelPalette = panel->palette();
    panelPalette.setColor(QPalette::Window, palette.backgroundColor);
    panel->setPalette(panelPalette);

    m_iconLabel->setPixmap(
        iconFromTheme(palette.iconThemeName).pixmap(ICON_SIZE, ICON_SIZE));

    QFont titleFont = m_titleLabel->font();
    titleFont.setBold(true);
    m_titleLabel->setFont(titleFont);
    m_titleLabel->setWordWrap(true);

    m_contentLabel->setWordWrap(true);

    m_closeButton->setAutoRaise(true);
    m_closeButton->setIcon(iconFromTheme("window-close"));
    m_closeButton->setToolTip(tr("Close"));
    connect(m_closeButton, &QToolButton::clicked, this,
            &NotificationBalloon::closed);

    auto* headerRow = new QHBoxLayout;
    headerRow->setContentsMargins(0, 0, 0, 0);
    headerRow->setSpacing(LAYOUT_SPACING);
    headerRow->addWidget(m_iconLabel);
    headerRow->addWidget(m_titleLabel, 1);
    headerRow->addWidget(m_closeButton);

    m_actionsLayout->setContentsMargins(0, 0, 0, 0);
    m_actionsLayout->setSpacing(LAYOUT_SPACING);
    m_actionsLayout->addStretch(1);

    auto* panelLayout = new QVBoxLayout(panel);
    panelLayout->setContentsMargins(CONTENT_MARGIN, CONTENT_MARGIN,
                                    CONTENT_MARGIN, CONTENT_MARGIN);
    panelLayout->setSpacing(LAYOUT_SPACING);
    panelLayout->addLayout(headerRow);
    panelLayout->addWidget(m_contentLabel);
    panelLayout->addLayout(m_actionsLayout);

    auto* topRow = new QHBoxLayout;
    topRow->setContentsMargins(0, 0, 0, 0);
    topRow->setSpacing(0);
    topRow->addWidget(stripe);
    topRow->addWidget(panel, 1);

    auto* rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);
    rootLayout->addLayout(topRow);

    if (!sticky) {
        m_countdownBar = new QProgressBar(this);
        m_countdownBar->setRange(0, DISMISS_MS);
        m_countdownBar->setValue(DISMISS_MS);
        m_countdownBar->setTextVisible(false);
        m_countdownBar->setFixedHeight(STRIPE_WIDTH);
        m_countdownBar->setStyleSheet(
            QStringLiteral("QProgressBar { border: none; background: "
                           "transparent; } QProgressBar::chunk { "
                           "background-color: %1; }")
                .arg(palette.stripeColor.name()));
        rootLayout->addWidget(m_countdownBar);

        m_remainingMs    = DISMISS_MS;
        m_countdownTimer = new QTimer(this);
        m_countdownTimer->setInterval(COUNTDOWN_TICK_MS);
        connect(m_countdownTimer, &QTimer::timeout, this,
                &NotificationBalloon::onCountdownTick);
        m_countdownTimer->start();
    }
}

void NotificationBalloon::addAction(const NotificationAction& action)
{
    if (m_inlineActionCount < MAX_INLINE_ACTIONS) {
        auto* button = new QToolButton(this);
        button->setAutoRaise(true);
        button->setCursor(Qt::PointingHandCursor);
        button->setText(action.title);
        button->setStyleSheet(
            QStringLiteral(
                "QToolButton { border: none; text-decoration: underline; "
                "color: %1; }")
                .arg(palette().color(QPalette::Link).name()));

        const auto handler = action.handler;
        connect(button, &QToolButton::clicked, this, [handler]() {
            if (handler) { handler(); }
        });

        // Inserted right before the trailing stretch.
        m_actionsLayout->insertWidget(m_actionsLayout->count() - 1, button);
        ++m_inlineActionCount;
        return;
    }

    if (m_moreButton == nullptr) {
        m_moreMenu   = new QMenu(this);
        m_moreButton = new QToolButton(this);
        m_moreButton->setAutoRaise(true);
        m_moreButton->setText(tr("More ▾"));
        m_moreButton->setPopupMode(QToolButton::InstantPopup);
        m_moreButton->setMenu(m_moreMenu);
        m_actionsLayout->insertWidget(m_actionsLayout->count() - 1,
                                      m_moreButton);
    }

    const auto handler = action.handler;
    connect(m_moreMenu->addAction(action.title), &QAction::triggered, this,
            [handler]() {
                if (handler) { handler(); }
            });
}

void NotificationBalloon::slideIn(const QPoint& fromPos, const QPoint& toPos)
{
    move(fromPos);
    show();

    m_slideAnimation = new QPropertyAnimation(this, "pos", this);
    m_slideAnimation->setDuration(SLIDE_ANIMATION_MS);
    m_slideAnimation->setStartValue(fromPos);
    m_slideAnimation->setEndValue(toPos);
    m_slideAnimation->setEasingCurve(QEasingCurve::OutCubic);
    m_slideAnimation->start(QAbstractAnimation::DeleteWhenStopped);
}

void NotificationBalloon::enterEvent(QEnterEvent* event)
{
    if (m_countdownTimer != nullptr) { m_countdownTimer->stop(); }
    QWidget::enterEvent(event);
}

void NotificationBalloon::leaveEvent(QEvent* event)
{
    if (m_countdownTimer != nullptr) { m_countdownTimer->start(); }
    QWidget::leaveEvent(event);
}

void NotificationBalloon::onCountdownTick()
{
    m_remainingMs -= COUNTDOWN_TICK_MS;
    if (m_remainingMs <= 0) {
        m_countdownTimer->stop();
        m_countdownBar->setValue(0);
        emit closed();
        return;
    }
    m_countdownBar->setValue(m_remainingMs);
}
