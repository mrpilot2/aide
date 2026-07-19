#ifndef AIDE_NOTIFICATION_POPUP_TYPE_DELEGATE_HPP
#define AIDE_NOTIFICATION_POPUP_TYPE_DELEGATE_HPP

#include <QStyledItemDelegate>

namespace aide::gui
{
    // Paints/edits NotificationGroupTableModel's Popup type column as a
    // combo box offering None / Balloon / Sticky balloon (#154). The editor
    // is kept open persistently by NotificationsSettingsPage so the combo is
    // always visible, matching the JetBrains Settings > Notifications look.
    class NotificationPopupTypeDelegate : public QStyledItemDelegate
    {
        Q_OBJECT

    public:
        explicit NotificationPopupTypeDelegate(QObject* parent = nullptr);

        [[nodiscard]] QWidget* createEditor(
            QWidget* parent, const QStyleOptionViewItem& option,
            const QModelIndex& index) const override;

        void setEditorData(QWidget* editor,
                           const QModelIndex& index) const override;

        void setModelData(QWidget* editor, QAbstractItemModel* model,
                          const QModelIndex& index) const override;

        void updateEditorGeometry(QWidget* editor,
                                  const QStyleOptionViewItem& option,
                                  const QModelIndex& index) const override;
    };
} // namespace aide::gui

#endif // AIDE_NOTIFICATION_POPUP_TYPE_DELEGATE_HPP
