

#ifndef AIDE_WIDGETS_MULTI_COLUMN_SORT_FILTER_PROXY_MODEL_HPP
#define AIDE_WIDGETS_MULTI_COLUMN_SORT_FILTER_PROXY_MODEL_HPP

#include <QSortFilterProxyModel>

namespace aide::widgets
{
    enum class FilterOption
    {
        Regex,
        Wildcard
    };

    class MultiColumnSortFilterProxyModel : public QSortFilterProxyModel
    {
    public:
        using QSortFilterProxyModel::QSortFilterProxyModel;

        void setFilterForColumn(int column, const QString& filterText);

        void setFilterOption(FilterOption option);

        void clearFilterForColumn(int column);

        void clearFilterForAllColumns();

        QRegularExpression getRegexForColumn(int column) const;

    protected:
        bool filterAcceptsRow(int source_row,
                              const QModelIndex& source_parent) const override;

    private:
        std::map<int, QString> m_columnFilterMap;

        FilterOption m_option{FilterOption::Regex};
    };
} // namespace aide::widgets

#endif // AIDE_WIDGETS_MULTI_COLUMN_SORT_FILTER_PROXY_MODEL_HPP
