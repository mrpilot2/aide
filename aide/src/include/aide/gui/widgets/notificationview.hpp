#ifndef AIDE_NOTIFICATION_VIEW_HPP
#define AIDE_NOTIFICATION_VIEW_HPP

#include <QWidget>

#include <aide/gui/widgets/aidewidgets_export.h>
#include <aide/hierarchicalid.hpp>

namespace aide
{
    class NotificationManagerInterface;
    class SettingsInterface;
} // namespace aide

class QLabel;
class QListView;
class QShowEvent;
class QToolButton;

namespace aide::core
{
    class NotificationLogModel;
} // namespace aide::core

namespace aide::widgets
{
    class NotificationLogItemDelegate;

    /**
     * @brief Plain embeddable QWidget showing the notification log (#155):
     * a JetBrains Notifications-timeline equivalent, NOT a docked tool
     * window. Flat, newest-first list backed by NotificationLogModel and
     * painted by NotificationLogItemDelegate.
     *
     * Read state is a boolean, not a count (#148's amendment to #145):
     * post() sets the manager's unread flag; this view clears it via the
     * global markRead() whenever it becomes visible, and again on every
     * post() that arrives while still visible. Consumers bind an unread dot
     * on whatever opener button toggles this view to
     * NotificationManagerInterface::hasUnread(); cards render uniform, with
     * no per-entry read/unread styling.
     *
     * The log is in-memory and session-only: nothing here is persisted, and
     * the timeline starts empty on every application launch.
     */
    class AIDEWIDGETS_EXPORT NotificationView : public QWidget
    {
        Q_OBJECT

    public:
        NotificationView(aide::NotificationManagerInterface& manager,
                         aide::SettingsInterface& settings,
                         QWidget* parent = nullptr);

    signals:
        // Header overflow menu's "Notification Settings..." entry.
        void settingsRequested();

        // Per-item overflow menu's "Settings" entry -- jump to groupId's
        // row in the notification settings page. Relayed from the
        // delegate; this widget has no SettingsDialog access itself.
        void groupSettingsRequested(const aide::HierarchicalId& groupId);

    protected:
        void showEvent(QShowEvent* event) override;

    private:
        void updateEmptyState();
        void toggleCollapsed();

        aide::NotificationManagerInterface& m_manager;

        QWidget* m_body{nullptr};
        QToolButton* m_collapseButton{nullptr};
        QListView* m_listView{nullptr};
        QLabel* m_emptyLabel{nullptr};
        aide::core::NotificationLogModel* m_model{nullptr};
        NotificationLogItemDelegate* m_delegate{nullptr};
    };
} // namespace aide::widgets

#endif // AIDE_NOTIFICATION_VIEW_HPP
