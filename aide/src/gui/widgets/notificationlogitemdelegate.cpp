#include "notificationlogitemdelegate.hpp"

#include <algorithm>
#include <limits>
#include <vector>

#include <QDateTime>
#include <QFontMetrics>
#include <QIcon>
#include <QMenu>
#include <QMouseEvent>
#include <QPainter>
#include <QStyleOptionViewItem>
#include <QWidget>

#include <aide/notification.hpp>
#include <aide/notificationdisplaytype.hpp>
#include <aide/notificationtype.hpp>
#include <aide/settingsinterface.hpp>

#include "notifications/notificationlogmodel.hpp"
#include "notificationtimestampformatter.hpp"
#include "settings/notifications/notificationsettingskeys.hpp"

using aide::Notification;
using aide::NotificationType;
using aide::core::NotificationLogModel;
using aide::widgets::NotificationLogItemDelegate;
using aide::widgets::NotificationTimestampFormatter;

namespace
{
    constexpr int STRIPE_WIDTH{3};
    constexpr int CONTENT_LEFT_PAD{14};
    constexpr int CONTENT_RIGHT_PAD{12};
    constexpr int CONTENT_TOP_PAD{10};
    constexpr int CONTENT_BOTTOM_PAD{11};
    constexpr int ICON_SIZE{16};
    constexpr int TOP_ROW_SPACING{9};
    constexpr int MENU_BUTTON_SIZE{18};
    constexpr int TIME_GAP{8};
    constexpr int ROW_SPACING{3};
    constexpr int ACTIONS_TOP_SPACING{8};
    constexpr int ACTIONS_GAP{14};
    constexpr int MAX_INLINE_ACTIONS{2};
    constexpr int MORE_ACTION_INDEX{-1};

    constexpr int INFORMATION_STRIPE_R{25};
    constexpr int INFORMATION_STRIPE_G{118};
    constexpr int INFORMATION_STRIPE_B{210};

    constexpr int SUCCESS_STRIPE_R{56};
    constexpr int SUCCESS_STRIPE_G{142};
    constexpr int SUCCESS_STRIPE_B{60};

    constexpr int WARNING_STRIPE_R{245};
    constexpr int WARNING_STRIPE_G{124};
    constexpr int WARNING_STRIPE_B{0};

    constexpr int ERROR_STRIPE_R{211};
    constexpr int ERROR_STRIPE_G{47};
    constexpr int ERROR_STRIPE_B{47};

    // Placeholder severity palette/icon-name table, duplicated from
    // NotificationBalloon's own placeholder (#152): all three surfaces
    // switch to AppearanceManager::severityColor() once #157 lands.
    struct SeverityPalette
    {
        QColor stripeColor;
        QString iconThemeName;
    };

    SeverityPalette paletteFor(NotificationType type)
    {
        switch (type) {
        case NotificationType::Success:
            return {.stripeColor   = QColor(SUCCESS_STRIPE_R, SUCCESS_STRIPE_G,
                                            SUCCESS_STRIPE_B),
                    .iconThemeName = "emblem-ok"};
        case NotificationType::Warning:
            return {.stripeColor   = QColor(WARNING_STRIPE_R, WARNING_STRIPE_G,
                                            WARNING_STRIPE_B),
                    .iconThemeName = "dialog-warning"};
        case NotificationType::Error:
            return {.stripeColor =
                        QColor(ERROR_STRIPE_R, ERROR_STRIPE_G, ERROR_STRIPE_B),
                    .iconThemeName = "dialog-error"};
        case NotificationType::Information:
        default:
            return {.stripeColor =
                        QColor(INFORMATION_STRIPE_R, INFORMATION_STRIPE_G,
                               INFORMATION_STRIPE_B),
                    .iconThemeName = "dialog-information"};
        }
    }

    QIcon iconFromTheme(const QString& name)
    {
        return QIcon::hasThemeIcon(name) ? QIcon::fromTheme(name) : QIcon();
    }

    struct ActionSlot
    {
        QRect rect;
        int actionIndex{MORE_ACTION_INDEX};
        QString text;
    };

    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init)
    struct ItemLayout
    {
        QRect iconRect;
        QRect titleRect;
        QRect menuRect;
        QRect timeRect;
        QRect bodyRect;
        std::vector<ActionSlot> actionSlots;
        QString timeText;
        int totalHeight{0};
    };

    ItemLayout computeLayout(const QStyleOptionViewItem& option,
                             const Notification& notification)
    {
        ItemLayout layout;
        const QRect& rect = option.rect;
        const QFontMetrics metrics(option.font);
        QFont boldFont = option.font;
        boldFont.setBold(true);
        const QFontMetrics boldMetrics(boldFont);

        const int contentLeft  = rect.left() + CONTENT_LEFT_PAD;
        const int contentRight = rect.right() - CONTENT_RIGHT_PAD;
        const int topRowY      = rect.top() + CONTENT_TOP_PAD;

        layout.iconRect = QRect(contentLeft, topRowY, ICON_SIZE, ICON_SIZE);

        layout.timeText = NotificationTimestampFormatter::format(
            notification.timestamp, QDateTime::currentDateTime());
        const int timeWidth = metrics.horizontalAdvance(layout.timeText);
        layout.timeRect =
            QRect(contentRight - timeWidth, topRowY, timeWidth, ICON_SIZE);

        layout.menuRect =
            QRect(layout.timeRect.left() - TIME_GAP - MENU_BUTTON_SIZE,
                  topRowY - 1, MENU_BUTTON_SIZE, MENU_BUTTON_SIZE);

        const int titleLeft  = layout.iconRect.right() + TOP_ROW_SPACING;
        const int titleRight = layout.menuRect.left() - TIME_GAP;
        layout.titleRect     = QRect(
            titleLeft, topRowY, std::max(0, titleRight - titleLeft), ICON_SIZE);

        const int bodyLeft  = titleLeft;
        const int bodyTop   = topRowY + ICON_SIZE + ROW_SPACING;
        const int bodyWidth = std::max(0, contentRight - bodyLeft);
        const QRect bodyBounds(bodyLeft, bodyTop, bodyWidth,
                               std::numeric_limits<int>::max() / 2);
        const QRect bodyFit = metrics.boundingRect(bodyBounds, Qt::TextWordWrap,
                                                   notification.content);
        layout.bodyRect = QRect(bodyLeft, bodyTop, bodyWidth, bodyFit.height());

        int cursorY = layout.bodyRect.bottom();
        if (!notification.actions.empty()) {
            cursorY += ACTIONS_TOP_SPACING;
            const int actionsHeight = boldMetrics.height();
            int cursorX             = bodyLeft;
            const auto actionCount =
                static_cast<int>(notification.actions.size());

            for (int i = 0; i < std::min(actionCount, MAX_INLINE_ACTIONS);
                 ++i) {
                const auto& action =
                    notification.actions[static_cast<std::size_t>(i)];
                const int width = metrics.horizontalAdvance(action.title);
                const QRect actionRect(cursorX, cursorY, width, actionsHeight);
                layout.actionSlots.push_back({actionRect, i, action.title});
                cursorX += width + ACTIONS_GAP;
            }

            if (actionCount > MAX_INLINE_ACTIONS) {
                const QString moreText =
                    NotificationLogItemDelegate::tr("More ▾");
                const int width = metrics.horizontalAdvance(moreText);
                layout.actionSlots.push_back(
                    {QRect(cursorX, cursorY, width, actionsHeight),
                     MORE_ACTION_INDEX, moreText});
            }

            cursorY += actionsHeight;
        }

        layout.totalHeight = cursorY + CONTENT_BOTTOM_PAD - rect.top();
        return layout;
    }
} // namespace

NotificationLogItemDelegate::NotificationLogItemDelegate(
    aide::SettingsInterface& settings, QObject* parent)
    : QStyledItemDelegate(parent)
    , m_settings(settings)
{}

void NotificationLogItemDelegate::paint(QPainter* painter,
                                        const QStyleOptionViewItem& option,
                                        const QModelIndex& index) const
{
    const auto notification = index.data(NotificationLogModel::NotificationRole)
                                  .value<Notification>();
    const auto palette = paletteFor(notification.type);
    const auto layout  = computeLayout(option, notification);

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);

    if ((option.state & QStyle::State_Selected) != 0U) {
        painter->fillRect(option.rect, option.palette.highlight());
    } else if ((option.state & QStyle::State_MouseOver) != 0U) {
        painter->fillRect(option.rect, option.palette.alternateBase());
    }

    const QRect stripeRect(option.rect.left(), option.rect.top(), STRIPE_WIDTH,
                           option.rect.height());
    painter->fillRect(stripeRect, palette.stripeColor);

    const QIcon icon = iconFromTheme(palette.iconThemeName);
    if (!icon.isNull()) { icon.paint(painter, layout.iconRect); }

    QFont boldFont = option.font;
    boldFont.setBold(true);
    painter->setFont(boldFont);
    painter->setPen(option.palette.color(QPalette::Text));
    painter->drawText(layout.titleRect, Qt::AlignVCenter | Qt::TextSingleLine,
                      notification.title);

    painter->setFont(option.font);
    painter->setPen(option.palette.color(QPalette::PlaceholderText));
    painter->drawText(layout.timeRect, Qt::AlignVCenter | Qt::AlignRight,
                      layout.timeText);
    painter->drawText(layout.menuRect, Qt::AlignCenter, QStringLiteral("⋮"));

    painter->setPen(option.palette.color(QPalette::Text));
    painter->drawText(layout.bodyRect, Qt::AlignLeft | Qt::TextWordWrap,
                      notification.content);

    painter->setPen(option.palette.link().color());
    for (const auto& slot : layout.actionSlots) {
        painter->drawText(slot.rect, Qt::AlignVCenter, slot.text);
    }

    painter->restore();
}

QSize NotificationLogItemDelegate::sizeHint(const QStyleOptionViewItem& option,
                                            const QModelIndex& index) const
{
    const auto notification = index.data(NotificationLogModel::NotificationRole)
                                  .value<Notification>();
    const auto layout = computeLayout(option, notification);
    return {option.rect.width(), layout.totalHeight};
}

bool NotificationLogItemDelegate::editorEvent(
    QEvent* event, QAbstractItemModel* /*model*/,
    const QStyleOptionViewItem& option, const QModelIndex& index)
{
    if (event->type() != QEvent::MouseButtonRelease) { return false; }

    auto* mouseEvent = dynamic_cast<QMouseEvent*>(event);
    if (mouseEvent == nullptr) { return false; }
    const QPoint pos = mouseEvent->pos();

    const auto notification = index.data(NotificationLogModel::NotificationRole)
                                  .value<Notification>();
    const auto layout = computeLayout(option, notification);

    if (layout.menuRect.contains(pos)) {
        const QPoint globalPos = mouseEvent->globalPosition().toPoint();
        QMenu menu;
        const QAction* settingsAction = menu.addAction(tr("Settings"));
        const QAction* muteAction =
            menu.addAction(tr("Turn off popups for this group"));

        const QAction* chosen = menu.exec(globalPos);
        if (chosen == settingsAction) {
            emit groupSettingsRequested(notification.groupId);
        } else if (chosen == muteAction) {
            m_settings.setValue(
                aide::core::notificationDisplayTypeKey(notification.groupId),
                static_cast<int>(aide::NotificationDisplayType::None));
        }
        return true;
    }

    for (const auto& slot : layout.actionSlots) {
        if (!slot.rect.contains(pos)) { continue; }

        if (slot.actionIndex == MORE_ACTION_INDEX) {
            QMenu menu;
            for (auto i = static_cast<std::size_t>(MAX_INLINE_ACTIONS);
                 i < notification.actions.size(); ++i) {
                const auto handler = notification.actions[i].handler;
                connect(menu.addAction(notification.actions[i].title),
                        &QAction::triggered, this, [handler]() {
                            if (handler) { handler(); }
                        });
            }
            menu.exec(mouseEvent->globalPosition().toPoint());
        } else {
            const auto& action =
                notification
                    .actions[static_cast<std::size_t>(slot.actionIndex)];
            if (action.handler) { action.handler(); }
        }
        return true;
    }

    return false;
}
