#include "notificationlogmodel.hpp"

#include <algorithm>

#include <QObject>

#include <aide/notificationmanagerinterface.hpp>

using aide::NotificationId;
using aide::core::NotificationLogModel;

NotificationLogModel::NotificationLogModel(
    aide::NotificationManagerInterface& manager, QObject* parent)
    : QAbstractListModel(parent)
    , m_manager(manager)
    , m_entries(manager.notifications())
{
    // notifications() is chronological ascending (monotonic id order);
    // reverse once here so the model itself is newest-first throughout.
    std::reverse(m_entries.begin(), m_entries.end());

    if (const auto* source = dynamic_cast<QObject*>(&m_manager)) {
        connect(source, SIGNAL(notificationPosted(aide::NotificationId)), this,
                SLOT(onNotificationPosted(aide::NotificationId)));
        connect(source, SIGNAL(notificationRemoved(aide::NotificationId)), this,
                SLOT(onNotificationRemoved(aide::NotificationId)));
        connect(source, SIGNAL(notificationUpdated(aide::NotificationId)), this,
                SLOT(onNotificationUpdated(aide::NotificationId)));
        connect(source, SIGNAL(notificationsCleared()), this,
                SLOT(onNotificationsCleared()));
    }
}

// NOLINTNEXTLINE
int NotificationLogModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid()) { return 0; }
    return static_cast<int>(m_entries.size());
}

QVariant NotificationLogModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid()) { return {}; }
    const auto row = static_cast<std::size_t>(index.row());
    if (row >= m_entries.size()) { return {}; }

    if (role == NotificationRole) {
        return QVariant::fromValue(m_entries[row]);
    }

    return {};
}

const aide::Notification& NotificationLogModel::notificationAt(int row) const
{
    return m_entries.at(static_cast<std::size_t>(row));
}

int NotificationLogModel::rowOf(NotificationId id) const
{
    const auto found =
        std::find_if(m_entries.begin(), m_entries.end(),
                     [id](const auto& entry) { return entry.id == id; });
    return found == m_entries.end()
               ? -1
               : static_cast<int>(std::distance(m_entries.begin(), found));
}

void NotificationLogModel::onNotificationPosted(NotificationId id)
{
    const auto notification = m_manager.notification(id);
    if (!notification.has_value()) { return; }

    beginInsertRows(QModelIndex(), 0, 0);
    m_entries.insert(m_entries.begin(), *notification);
    endInsertRows();
}

void NotificationLogModel::onNotificationRemoved(NotificationId id)
{
    const int row = rowOf(id);
    if (row < 0) { return; }

    beginRemoveRows(QModelIndex(), row, row);
    m_entries.erase(m_entries.begin() + row);
    endRemoveRows();
}

void NotificationLogModel::onNotificationUpdated(NotificationId id)
{
    const int row = rowOf(id);
    if (row < 0) { return; }

    const QModelIndex changed = index(row);
    emit dataChanged(changed, changed);
}

void NotificationLogModel::onNotificationsCleared()
{
    beginResetModel();
    m_entries.clear();
    endResetModel();
}
