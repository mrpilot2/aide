#include "settingspagefilterproxymodel.hpp"

#include <QString>

using aide::core::SettingsPageFilterProxyModel;

SettingsPageFilterProxyModel::SettingsPageFilterProxyModel(QObject* parent)
    : QSortFilterProxyModel(parent)
{
    setRecursiveFilteringEnabled(true);
    setFilterCaseSensitivity(Qt::CaseInsensitive);
    setFilterRole(Qt::DisplayRole);
}

void SettingsPageFilterProxyModel::setSearchPattern(const QString& pattern)
{
    setFilterFixedString(pattern);
}
