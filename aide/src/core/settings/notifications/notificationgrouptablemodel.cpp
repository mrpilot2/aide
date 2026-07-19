#include "notificationgrouptablemodel.hpp"

#include <algorithm>
#include <utility>

#include <QColor>

#include "settings/searchpattern.hpp"
#include "settings/textmatcher.hpp"

using aide::NotificationDisplayType;
using aide::NotificationGroup;
using aide::core::NotificationGroupTableModel;

namespace
{
    constexpr int HIGHLIGHT_RED   = 255;
    constexpr int HIGHLIGHT_GREEN = 235;
    constexpr int HIGHLIGHT_BLUE  = 205;

    QString groupLabel(const NotificationGroup& group)
    {
        return group.displayName.isEmpty()
                   ? QString::fromStdString(group.id.name())
                   : group.displayName;
    }
} // namespace

NotificationGroupTableModel::NotificationGroupTableModel(QObject* parent)
    : QAbstractTableModel(parent)
{}

void NotificationGroupTableModel::setGroups(
    std::vector<NotificationGroup> groups)
{
    beginResetModel();
    m_rows.clear();
    m_rows.reserve(groups.size());
    for (auto& group : groups) {
        const auto defaultType = group.defaultDisplayType;
        m_rows.push_back(
            Row{.group = std::move(group), .displayType = defaultType});
    }
    endResetModel();
}

// NOLINTNEXTLINE
int NotificationGroupTableModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid()) { return 0; }
    return static_cast<int>(m_rows.size());
}

// NOLINTNEXTLINE
int NotificationGroupTableModel::columnCount(const QModelIndex& parent) const
{
    if (parent.isValid()) { return 0; }
    return ColumnCount;
}

QVariant NotificationGroupTableModel::data(const QModelIndex& index,
                                           int role) const
{
    if (!index.isValid()) { return {}; }
    const auto rowIndex = static_cast<size_t>(index.row());
    if (rowIndex >= m_rows.size()) { return {}; }

    const auto& row = m_rows[rowIndex];

    if (role == Qt::ToolTipRole && index.column() == GroupColumn) {
        return QString::fromStdString(row.group.id.name());
    }

    if (role == Qt::DisplayRole) {
        switch (index.column()) {
        case GroupColumn:
            return groupLabel(row.group);
        case PopupTypeColumn:
            return displayTypeLabel(row.displayType);
        case LogColumn:
            return tr("On");
        default:
            return {};
        }
    }

    if (role == Qt::EditRole && index.column() == PopupTypeColumn) {
        return static_cast<int>(row.displayType);
    }

    if (role == Qt::BackgroundRole && !m_searchPattern.isEmpty()) {
        const auto words = tokenizeSearchPattern(m_searchPattern);
        if (rowMatches(row, words)) {
            return QColor(HIGHLIGHT_RED, HIGHLIGHT_GREEN, HIGHLIGHT_BLUE);
        }
    }

    return {};
}

bool NotificationGroupTableModel::setData(const QModelIndex& index,
                                          const QVariant& value, int role)
{
    if (!index.isValid() || index.column() != PopupTypeColumn ||
        role != Qt::EditRole) {
        return false;
    }

    const auto rowIndex = static_cast<size_t>(index.row());
    if (rowIndex >= m_rows.size()) { return false; }

    m_rows[rowIndex].displayType =
        static_cast<NotificationDisplayType>(value.toInt());
    emit dataChanged(index, index, {Qt::DisplayRole, Qt::EditRole});
    return true;
}

Qt::ItemFlags NotificationGroupTableModel::flags(const QModelIndex& index) const
{
    if (!index.isValid()) { return Qt::NoItemFlags; }

    auto itemFlags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    if (index.column() == PopupTypeColumn) { itemFlags |= Qt::ItemIsEditable; }
    return itemFlags;
}

QVariant NotificationGroupTableModel::headerData(int section,
                                                 Qt::Orientation orientation,
                                                 int role) const
{
    if (role != Qt::DisplayRole || orientation != Qt::Horizontal) {
        return QAbstractTableModel::headerData(section, orientation, role);
    }

    switch (section) {
    case GroupColumn:
        return tr("Group");
    case PopupTypeColumn:
        return tr("Popup type");
    case LogColumn:
        return tr("Log");
    default:
        return {};
    }
}

const NotificationGroup& NotificationGroupTableModel::groupAt(int row) const
{
    return m_rows.at(static_cast<size_t>(row)).group;
}

NotificationDisplayType NotificationGroupTableModel::displayTypeAt(
    int row) const
{
    return m_rows.at(static_cast<size_t>(row)).displayType;
}

void NotificationGroupTableModel::setDisplayTypeAt(int row,
                                                   NotificationDisplayType type)
{
    setData(index(row, PopupTypeColumn), static_cast<int>(type), Qt::EditRole);
}

bool NotificationGroupTableModel::matchesAnyRow(const QStringList& words) const
{
    return std::ranges::any_of(
        m_rows, [&words](const Row& row) { return rowMatches(row, words); });
}

double NotificationGroupTableModel::rowsScore(const QStringList& words) const
{
    QStringList texts;
    texts.reserve(static_cast<qsizetype>(m_rows.size()) * 2);
    for (const auto& row : m_rows) {
        texts << QString::fromStdString(row.group.id.name());
        if (!row.group.displayName.isEmpty()) {
            texts << row.group.displayName;
        }
    }
    return TextMatcher::score(texts, words);
}

void NotificationGroupTableModel::setSearchPattern(const QString& pattern)
{
    m_searchPattern = pattern;
    if (m_rows.empty()) { return; }

    const auto topLeft = index(0, 0);
    const auto bottomRight =
        index(static_cast<int>(m_rows.size()) - 1, ColumnCount - 1);
    emit dataChanged(topLeft, bottomRight, {Qt::BackgroundRole});
}

QString NotificationGroupTableModel::displayTypeLabel(
    NotificationDisplayType type)
{
    switch (type) {
    case NotificationDisplayType::None:
        return tr("None");
    case NotificationDisplayType::Balloon:
        return tr("Balloon");
    case NotificationDisplayType::StickyBalloon:
        return tr("Sticky balloon");
    }
    return {};
}

bool NotificationGroupTableModel::rowMatches(const Row& row,
                                             const QStringList& words)
{
    return matchesAnyWord(QString::fromStdString(row.group.id.name()), words) ||
           matchesAnyWord(row.group.displayName, words);
}
