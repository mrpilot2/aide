
#include "banner.hpp"

#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QPalette>
#include <QToolButton>

using aide::NotificationAction;
using aide::NotificationType;
using aide::widgets::Banner;

namespace
{
    constexpr int STRIPE_WIDTH{4};
    constexpr int CONTENT_MARGIN{8};
    constexpr int LAYOUT_SPACING{8};
    constexpr int ICON_SIZE{16};

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

    struct BannerPalette
    {
        QColor stripeColor;
        QColor backgroundColor;
        QString iconThemeName;
    };

    // Placeholder palette/icon-name table: severity colours and icons draw
    // from the aIDE light/dark icon themes once #157 lands per-type assets;
    // until then these are freedesktop icon-naming-spec names resolved
    // against whatever platform theme is active, with a guarded empty
    // fallback (see iconFromTheme below).
    BannerPalette paletteFor(NotificationType type)
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

Banner::Banner(NotificationType type, const QString& message, QWidget* parent)
    : QFrame(parent)
    , m_iconLabel(new QLabel(this))
    , m_messageLabel(new QLabel(message, this))
    , m_closeButton(new QToolButton(this))
    , m_contentLayout(new QHBoxLayout)
{
    const auto palette = paletteFor(type);

    auto* stripe = new QWidget(this);
    stripe->setFixedWidth(STRIPE_WIDTH);
    stripe->setAutoFillBackground(true);
    QPalette stripePalette = stripe->palette();
    stripePalette.setColor(QPalette::Window, palette.stripeColor);
    stripe->setPalette(stripePalette);

    setAutoFillBackground(true);
    QPalette framePalette = this->palette();
    framePalette.setColor(QPalette::Window, palette.backgroundColor);
    setPalette(framePalette);

    m_iconLabel->setPixmap(
        iconFromTheme(palette.iconThemeName).pixmap(ICON_SIZE, ICON_SIZE));

    m_messageLabel->setWordWrap(true);

    m_closeButton->setAutoRaise(true);
    m_closeButton->setIcon(iconFromTheme("window-close"));
    m_closeButton->setToolTip(tr("Close"));
    connect(m_closeButton, &QToolButton::clicked, this, &Banner::closed);

    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(stripe);

    m_contentLayout->setContentsMargins(CONTENT_MARGIN, CONTENT_MARGIN,
                                        CONTENT_MARGIN, CONTENT_MARGIN);
    m_contentLayout->setSpacing(LAYOUT_SPACING);
    m_contentLayout->addWidget(m_iconLabel);
    m_contentLayout->addWidget(m_messageLabel, 1);
    m_contentLayout->addWidget(m_closeButton);
    layout->addLayout(m_contentLayout, 1);
}

void Banner::addAction(const NotificationAction& action)
{
    auto* button = new QToolButton(this);
    button->setAutoRaise(true);
    button->setCursor(Qt::PointingHandCursor);
    button->setText(action.title);
    // "Link-style": no button chrome, underlined text in the palette's link
    // colour, matching how the platform theme renders hyperlinks.
    button->setStyleSheet(
        QStringLiteral(
            "QToolButton { border: none; text-decoration: underline; "
            "color: %1; }")
            .arg(palette().color(QPalette::Link).name()));

    const auto handler = action.handler;
    connect(button, &QToolButton::clicked, this, [handler]() {
        if (handler) { handler(); }
    });

    // Inserted right before the close button, which must stay the last
    // widget in the strip.
    m_contentLayout->insertWidget(m_contentLayout->count() - 1, button);
}

void Banner::setClosable(bool closable)
{
    m_closeButton->setVisible(closable);
}
