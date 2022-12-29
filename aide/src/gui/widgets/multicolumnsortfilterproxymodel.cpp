

#include "multicolumnsortfilterproxymodel.hpp"

#include <iostream>

using aide::widgets::MultiColumnSortFilterProxyModel;

void MultiColumnSortFilterProxyModel::setFilterForColumn(
    int column, const QString& filterText)
{
    if (m_columnFilterMap.count(column) > 0) {
        m_columnFilterMap.at(column) =
            m_columnFilterMap.at(column) + '|' + filterText;
    } else {
        m_columnFilterMap.try_emplace(column, filterText);
    }
    invalidateFilter();
}

void MultiColumnSortFilterProxyModel::clearFilterForAllColumns()
{
    m_columnFilterMap.clear();
}

void MultiColumnSortFilterProxyModel::clearFilterForColumn(int column)
{
    if (m_columnFilterMap.count(column) > 0) {
        m_columnFilterMap.erase(column);
    }
}

bool aide::widgets::MultiColumnSortFilterProxyModel::filterAcceptsRow(
    int source_row, const QModelIndex& source_parent) const
{
    if (m_columnFilterMap.empty()) {
        return QSortFilterProxyModel::filterAcceptsRow(source_row,
                                                       source_parent);
    }

    bool result = true;
    for (auto const& [column_index, filterText] : m_columnFilterMap) {
        const QRegularExpression regex(filterText);

        if (column_index == -1) {
            bool anyColumnMatches = false;
            for (int i = 0; i < sourceModel()->columnCount(); ++i) {
                auto modelIndex =
                    sourceModel()->index(source_row, i, source_parent);

                auto value = sourceModel()->data(modelIndex).toString();

                anyColumnMatches =
                    anyColumnMatches || regex.match(value).hasMatch();
            }
            result = result && anyColumnMatches;
        } else {
            auto modelIndex =
                sourceModel()->index(source_row, column_index, source_parent);

            auto value = sourceModel()->data(modelIndex).toString();

            result = result && regex.match(value).hasMatch();
        }
    }

    return result;
}
