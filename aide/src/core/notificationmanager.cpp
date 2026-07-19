#include "aide/notificationmanager.hpp"

#include <utility>

#include "aide/settingsinterface.hpp"
#include "settings/notifications/notificationsettingskeys.hpp"

using aide::HierarchicalId;
using aide::Notification;
using aide::NotificationDisplayType;
using aide::NotificationGroup;
using aide::NotificationId;
using aide::NotificationManager;
using aide::SettingsInterface;

NotificationManager::NotificationManager(SettingsInterface& settings,
                                         QObject* parent)
    : QObject(parent)
    , m_settings(settings)
{}

void NotificationManager::registerGroup(NotificationGroup group)
{
    const auto id = group.id;
    m_groups.insert_or_assign(id, std::move(group));
}

std::optional<NotificationGroup> NotificationManager::group(
    const HierarchicalId& id) const
{
    if (const auto it = m_groups.find(id); it != m_groups.end()) {
        return it->second;
    }
    return {};
}

std::vector<NotificationGroup> NotificationManager::groups() const
{
    std::vector<NotificationGroup> result;
    result.reserve(m_groups.size());
    for (const auto& [id, notificationGroup] : m_groups) {
        result.push_back(notificationGroup);
    }
    return result;
}

NotificationId NotificationManager::post(Notification notification)
{
    const NotificationId id{m_nextId++};
    notification.id        = id;
    notification.timestamp = QDateTime::currentDateTime();

    const auto resolved = resolveDisplayType(notification.groupId);

    m_log.emplace(id, LogEntry{.notification        = std::move(notification),
                               .resolvedDisplayType = resolved,
                               .read                = false});

    setHasUnread(true);

    emit notificationPosted(id);
    return id;
}

void NotificationManager::remove(NotificationId id)
{
    if (m_log.erase(id) > 0) { emit notificationRemoved(id); }
}

void NotificationManager::markRead(NotificationId id)
{
    if (const auto it = m_log.find(id); it != m_log.end()) {
        it->second.read = true;
        emit notificationUpdated(id);
    }
}

void NotificationManager::clearAll()
{
    m_log.clear();
    emit notificationsCleared();
}

std::vector<Notification> NotificationManager::notifications() const
{
    std::vector<Notification> result;
    result.reserve(m_log.size());
    for (const auto& [id, entry] : m_log) {
        result.push_back(entry.notification);
    }
    return result;
}

std::optional<Notification> NotificationManager::notification(
    NotificationId id) const
{
    if (const auto it = m_log.find(id); it != m_log.end()) {
        return it->second.notification;
    }
    return {};
}

bool NotificationManager::isRead(NotificationId id) const
{
    if (const auto it = m_log.find(id); it != m_log.end()) {
        return it->second.read;
    }
    return false;
}

NotificationDisplayType NotificationManager::resolvedDisplayType(
    NotificationId id) const
{
    if (const auto it = m_log.find(id); it != m_log.end()) {
        return it->second.resolvedDisplayType;
    }
    return NotificationDisplayType::None;
}

bool NotificationManager::doNotDisturb() const
{
    return m_settings.value(core::notificationDoNotDisturbKey(), false)
        .toBool();
}

void NotificationManager::setDoNotDisturb(bool enabled)
{
    m_settings.setValue(core::notificationDoNotDisturbKey(), enabled);
}

bool NotificationManager::hasUnread() const
{
    return m_hasUnread;
}

void NotificationManager::markRead()
{
    setHasUnread(false);
}

NotificationDisplayType NotificationManager::resolveDisplayType(
    const HierarchicalId& groupId) const
{
    if (doNotDisturb()) { return NotificationDisplayType::None; }

    const auto overrideValue =
        m_settings.value(core::notificationDisplayTypeKey(groupId));
    if (overrideValue.isValid()) {
        return static_cast<NotificationDisplayType>(overrideValue.toInt());
    }

    if (const auto grp = group(groupId)) { return grp->defaultDisplayType; }

    return NotificationDisplayType::None;
}

void NotificationManager::setHasUnread(bool unread)
{
    if (m_hasUnread == unread) { return; }
    m_hasUnread = unread;
    emit unreadChanged(m_hasUnread);
}
