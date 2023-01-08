
#ifndef AIDE_SEARCH_FILTER_RESULT_DELEGATE_HPP
#define AIDE_SEARCH_FILTER_RESULT_DELEGATE_HPP

#include <QItemDelegate>

class QModelIndex;
class QPainter;
class QStyleOptionViewItem;

namespace aide::widgets
{
    class SearchFilterResultDelegate : public QItemDelegate
    {
    public:
        using QItemDelegate::QItemDelegate;

    protected:
        void paint(QPainter* painter, const QStyleOptionViewItem& option,
                   const QModelIndex& index) const override;
        void drawDisplay(QPainter* painter, const QStyleOptionViewItem& option,
                         const QRect& rect, const QString& text) const override;

    private:
        void highlightMatches(const QRegularExpression& expression, QPainter* painter,
                               const QStyleOptionViewItem& option, const QRect& rect) const;

        mutable QString m_currentValue;
        mutable QRegularExpression m_expr;

        mutable QRegularExpression m_exprForAllColumns;
    };
} // namespace aide::widgets

#endif // AIDE_SEARCH_FILTER_RESULT_DELEGATE_HPP