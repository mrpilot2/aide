
#include "searchfilterresultdelegate.hpp"

#include <iostream>

#include <QModelIndex>
#include <QPainter>
#include <QRegularExpression>
#include <QSortFilterProxyModel>
#include <QString>
#include <QTextCharFormat>
#include <QTextCursor>
#include <QTextDocument>

#include "multicolumnsortfilterproxymodel.hpp"

using aide::widgets::MultiColumnSortFilterProxyModel;
using aide::widgets::SearchFilterResultDelegate;

void SearchFilterResultDelegate::paint(QPainter* painter,
                                       const QStyleOptionViewItem& option,
                                       const QModelIndex& index) const
{
    if (!index.isValid()) { return; }

    const auto* const filterProxyModel =
        dynamic_cast<const MultiColumnSortFilterProxyModel*>(index.model());

    if (filterProxyModel == nullptr) {
        QItemDelegate::paint(painter, option, index);

        return;
    }

    m_currentValue = index.model()->data(index, Qt::DisplayRole).toString();

    m_expr              = filterProxyModel->getRegexForColumn(index.column());
    m_exprForAllColumns = filterProxyModel->getRegexForColumn(-1);

    QItemDelegate::paint(painter, option, index);
}

void SearchFilterResultDelegate::drawDisplay(QPainter* painter,
                                             const QStyleOptionViewItem& option,
                                             const QRect& rect,
                                             const QString& text) const
{
    highlightMatches(m_expr, painter, option, rect);
    highlightMatches(m_exprForAllColumns, painter, option, rect);

    QItemDelegate::drawDisplay(painter, option, rect, text);
}

void SearchFilterResultDelegate::highlightMatches(
    const QRegularExpression& expression, QPainter* painter,
    const QStyleOptionViewItem& option, const QRect& rect) const
{
    const auto fontMetrics{option.fontMetrics};

    if (expression.isValid() && !expression.pattern().isEmpty() &&
        !expression.pattern().endsWith('|')) {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        auto iterator = expression.globalMatch(m_currentValue);
        while (iterator.hasNext()) {
            auto match = iterator.next();
#else
        for (const auto& match : expression.globalMatch(m_currentValue)) {
#endif

#if QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
            auto startPos = fontMetrics.horizontalAdvance(
                m_currentValue.mid(0, match.capturedStart()));
            auto endPos =
                startPos + fontMetrics.horizontalAdvance(m_currentValue.mid(
                               match.capturedStart(),
                               match.capturedEnd() - match.capturedStart()));
#else
            auto startPos =
                fontMetrics.width(m_currentValue.mid(0, match.capturedStart()));
            auto endPos =
                startPos + fontMetrics.width(m_currentValue.mid(
                               match.capturedStart(),
                               match.capturedEnd() - match.capturedStart()));
#endif

            auto rectStartPos{std::min(rect.left() + startPos, rect.right())};
            auto rectEndPos{std::min(rect.left() + endPos, rect.right())};

            painter->save();
            const QRect paintRect(QPoint(rectStartPos, rect.top()),
                                  QPoint(rectEndPos + 2, rect.bottom()));
            painter->fillRect(paintRect, Qt::darkYellow);
            painter->restore();
        }
    }
}
