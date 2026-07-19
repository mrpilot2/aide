#ifndef AIDE_NOTIFICATION_MANAGER_HPP
#define AIDE_NOTIFICATION_MANAGER_HPP

#include <map>

#include <QObject>

#include <aide/hierarchicalid.hpp>
#include <aide/notificationmanagerinterface.hpp>

namespace aide
{
    class SettingsInterface;

    class NotificationManager
        : public QObject
        , public NotificationManagerInterface
    {
        Q_OBJECT

    public:
        explicit NotificationManager(SettingsInterface& settings,
                                     QObject* parent = nullptr);

        void registerGroup(NotificationGroup group) override;

        [[nodiscard]] std::optional<NotificationGroup> group(
            const HierarchicalId& id) const override;

        [[nodiscard]] std::vector<NotificationGroup> groups() const override;

        NotificationId post(Notification notification) override;

        void remove(NotificationId id) override;

        void markRead(NotificationId id) override;

        void clearAll() override;

        [[nodiscard]] std::vector<Notification> notifications() const override;

        [[nodiscard]] std::optional<Notification> notification(
            NotificationId id) const override;

        [[nodiscard]] bool isRead(NotificationId id) const override;

        [[nodiscard]] NotificationDisplayType resolvedDisplayType(
            NotificationId id) const override;

        [[nodiscard]] bool doNotDisturb() const override;

        void setDoNotDisturb(bool enabled) override;

        [[nodiscard]] bool hasUnread() const override;

        void markRead() override;

    signals:
        void notificationPosted(aide::NotificationId id);
        void notificationUpdated(aide::NotificationId id);
        void notificationRemoved(aide::NotificationId id);
        void notificationsCleared();
        void unreadChanged(bool hasUnread);

    private:
        struct LogEntry
        {
            Notification notification;
            NotificationDisplayType resolvedDisplayType{
                NotificationDisplayType::None};
            bool read{false};
        };

        [[nodiscard]] NotificationDisplayType resolveDisplayType(
            const HierarchicalId& groupId) const;

        void setHasUnread(bool unread);

        SettingsInterface& m_settings;

        std::map<HierarchicalId, NotificationGroup> m_groups;
        std::map<NotificationId, LogEntry> m_log;

        quint64 m_nextId{1};
        bool m_hasUnread{false};
    };
} // namespace aide

#endif // AIDE_NOTIFICATION_MANAGER_HPP
