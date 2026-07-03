#include "settingspagefilterproxymodel.hpp"

#include <QModelIndex>

#include "settingspagegrouptreemodel.hpp"

using aide::core::SettingsPageFilterProxyModel;

SettingsPageFilterProxyModel::SettingsPageFilterProxyModel(QObject* parent)
    : QSortFilterProxyModel(parent)
{
    setRecursiveFilteringEnabled(true);
}

void SettingsPageFilterProxyModel::setSearchPattern(const QString& pattern)
{
    m_searchPattern = pattern;
    invalidateFilter();
}

bool SettingsPageFilterProxyModel::filterAcceptsRow(
    const int sourceRow, const QModelIndex& sourceParent) const
{
    if (m_searchPattern.isEmpty()) { return true; }

    const auto index = sourceModel()->index(sourceRow, 0, sourceParent);

    const auto displayName =
        sourceModel()->data(index, Qt::DisplayRole).toString();
    if (displayName.contains(m_searchPattern, Qt::CaseInsensitive)) {
        return true;
    }

    if (const auto* treeModel =
            qobject_cast<const SettingsPageGroupTreeModel*>(sourceModel())) {
        if (const auto page = treeModel->findCorrespondingSettingsPage(index)) {
            return page->matches(m_searchPattern);
        }
    }

    return false;
}
