

#include "multicolumnsortfilterproxymodel.hpp"

#include <QCoreApplication>
#include <QRegularExpression>

using aide::widgets::MultiColumnSortFilterProxyModel;

void MultiColumnSortFilterProxyModel::setFilterForColumn(
    const int column, const QString& filterText)
{
    if (m_columnFilterMap.contains(column)) {
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
    invalidateFilter();
}

void MultiColumnSortFilterProxyModel::clearFilterForColumn(const int column)
{
    if (m_columnFilterMap.contains(column)) {
        m_columnFilterMap.erase(column);
        invalidateFilter();
    }
}

void MultiColumnSortFilterProxyModel::setFilterOption(const FilterOption option)
{
    m_option = option;
}

bool MultiColumnSortFilterProxyModel::filterAcceptsRow(
    const int source_row, const QModelIndex& source_parent) const
{
    if (m_columnFilterMap.empty()) {
        return QSortFilterProxyModel::filterAcceptsRow(source_row,
                                                       source_parent);
    }

    bool result = true;
    for (const auto& [column_index, filterText] : m_columnFilterMap) {
        auto regex = getRegexForColumn(column_index);

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

QRegularExpression MultiColumnSortFilterProxyModel::getRegexForColumn(
    const int column) const
{
    if (!m_columnFilterMap.contains(column)) { return {}; }

    auto filterText = m_columnFilterMap.at(column);

    QRegularExpression regex(filterText);

    if (m_option == FilterOption::Wildcard) {
        regex = QRegularExpression::fromWildcard(filterText,
                                                 filterCaseSensitivity());
    }
    if (filterCaseSensitivity() == Qt::CaseInsensitive) {
        regex.setPatternOptions(regex.patternOptions().setFlag(
            QRegularExpression::CaseInsensitiveOption));
    } else {
        regex.setPatternOptions(regex.patternOptions().setFlag(
            QRegularExpression::CaseInsensitiveOption, false));
    }

    return regex;
}
