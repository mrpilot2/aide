#ifndef AIDE_NOTIFICATION_LOG_MODEL_HPP
#define AIDE_NOTIFICATION_LOG_MODEL_HPP

#include <vector>

#include <QAbstractListModel>

#include <aide/notification.hpp>
#include <aide/notificationid.hpp>

namespace aide
{
    class NotificationManagerInterface;
} // namespace aide

namespace aide::core
{
    // Flat, newest-first backing model for the notification-view widget
    // (#155). Queries NotificationManagerInterface for the initial log, then
    // stays live by connecting to the concrete NotificationManager's signals
    // -- the interface itself carries none (#145) -- via a dynamic_cast to
    // QObject, the same string-based-connect DLL-safety convention used at
    // ApplicationBuilder wiring time, so this model works standalone for any
    // consumer holding only a NotificationManagerInterface&.
    class NotificationLogModel : public QAbstractListModel
    {
        Q_OBJECT

    public:
        enum Role : int
        {
            NotificationRole = Qt::UserRole + 1
        };

        explicit NotificationLogModel(
            aide::NotificationManagerInterface& manager,
            QObject* parent = nullptr);

        [[nodiscard]] int rowCount(
            const QModelIndex& parent = QModelIndex()) const override;

        [[nodiscard]] QVariant data(const QModelIndex& index,
                                    int role) const override;

        [[nodiscard]] const aide::Notification& notificationAt(int row) const;

    private:
        [[nodiscard]] int rowOf(aide::NotificationId id) const;

        aide::NotificationManagerInterface& m_manager;
        std::vector<aide::Notification> m_entries;

    private slots:
        void onNotificationPosted(aide::NotificationId id);
        void onNotificationRemoved(aide::NotificationId id);
        void onNotificationUpdated(aide::NotificationId id);
        void onNotificationsCleared();
    };
} // namespace aide::core

#endif // AIDE_NOTIFICATION_LOG_MODEL_HPP
