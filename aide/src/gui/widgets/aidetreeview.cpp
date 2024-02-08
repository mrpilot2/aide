
#include "widgets/aidetreeview.hpp"

#include <utility>

#include <QApplication>
#include <QBrush>
#include <QPainter>
#include <QPalette>
#include <QSortFilterProxyModel>

using aide::widgets::AideTreeView;

AideTreeView::AideTreeView(QWidget* parent)
    : QTreeView(parent)
    , placeHolderText{QApplication::translate(
          "AideTreeView", "There are no elements in this view")}
{}

void AideTreeView::paintEvent(QPaintEvent* event)
{
    QString textToDisplay(placeHolderText);
    if (model() != nullptr && model()->rowCount() > 0) {
        QTreeView::paintEvent(event);
        return;
    }

    auto const* sourceModel = model();
    if (auto const* filterModel = qobject_cast<QSortFilterProxyModel*>(model());
        filterModel != nullptr) {
        sourceModel = filterModel->sourceModel();
    }

    if (sourceModel != nullptr && model() != nullptr &&
        model()->rowCount() != sourceModel->rowCount()) {
        textToDisplay.append(
            QApplication::translate("AideTableView", " due to active filter"));
    }

    QPainter painter(viewport());

#if QT_VERSION < QT_VERSION_CHECK(5, 12, 0)
    const auto placeHolderTextAlpha{128};
    QColor textColor{palette().text().color()};
    textColor.setAlpha(placeHolderTextAlpha);
    painter.setPen(textColor);
#else
    painter.setPen(palette().placeholderText().color());
#endif

    QRect textRect = painter.fontMetrics().boundingRect(placeHolderText);
    textRect.moveCenter(viewport()->rect().center());
    painter.drawText(textRect, Qt::AlignCenter, placeHolderText);
}

void AideTreeView::setPlaceHolderText(QString text)
{
    placeHolderText = std::move(text);
}
