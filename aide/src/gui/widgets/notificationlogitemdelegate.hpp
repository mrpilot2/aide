#ifndef AIDE_NOTIFICATION_LOG_ITEM_DELEGATE_HPP
#define AIDE_NOTIFICATION_LOG_ITEM_DELEGATE_HPP

#include <QStyledItemDelegate>

#include <aide/hierarchicalid.hpp>

namespace aide
{
    class SettingsInterface;
} // namespace aide

namespace aide::widgets
{
    // Paints one notification-log card (#155): severity stripe + type icon,
    // bold title, right-aligned absolute timestamp, multi-line body, up to
    // two inline action links + "More" overflow (same cap as the balloon,
    // #147), and a per-item overflow menu (jump to the group's settings row
    // / turn off popups for the group). None of this is real child widgets
    // -- editorEvent() hit-tests the same rects paint() draws.
    class NotificationLogItemDelegate : public QStyledItemDelegate
    {
        Q_OBJECT

    public:
        explicit NotificationLogItemDelegate(aide::SettingsInterface& settings,
                                             QObject* parent = nullptr);

        void paint(QPainter* painter, const QStyleOptionViewItem& option,
                   const QModelIndex& index) const override;

        [[nodiscard]] QSize sizeHint(const QStyleOptionViewItem& option,
                                     const QModelIndex& index) const override;

        bool editorEvent(QEvent* event, QAbstractItemModel* model,
                         const QStyleOptionViewItem& option,
                         const QModelIndex& index) override;

    signals:
        // Consumer wires this to open the notification settings page and
        // select groupId's row; the delegate has no SettingsDialog access.
        void groupSettingsRequested(const aide::HierarchicalId& groupId);

    private:
        aide::SettingsInterface& m_settings;
    };
} // namespace aide::widgets

#endif // AIDE_NOTIFICATION_LOG_ITEM_DELEGATE_HPP
