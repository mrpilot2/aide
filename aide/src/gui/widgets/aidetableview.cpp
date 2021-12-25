//
// Created by markus on 5/3/21.
//

#include "widgets/aidetableview.hpp"

#include <utility>

#include <QApplication>
#include <QBrush>
#include <QPainter>
#include <QPalette>
#include <QSortFilterProxyModel>

using aide::widgets::AideTableView;

AideTableView::AideTableView(QWidget* parent)
    : QTableView(parent)
    , placeHolderText{QApplication::translate(
          "AideTableView", "There are no elements in this view")}
{}

void AideTableView::paintEvent(QPaintEvent* event)
{
    QString textToDisplay(placeHolderText);
    if ((model() != nullptr) &&
        (model()->rowCount() > 0 && model()->columnCount() > 0)) {
        QTableView::paintEvent(event);
        return;
    }

    auto const* sourceModel = model();
    if (auto const* filterModel = qobject_cast<QSortFilterProxyModel*>(model());
        filterModel != nullptr) {
        sourceModel = filterModel->sourceModel();
    }

    if (sourceModel != nullptr && model() != nullptr &&
        (model()->rowCount() != sourceModel->rowCount())) {
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

    QRect textRect = painter.fontMetrics().boundingRect(textToDisplay);
    textRect.moveCenter(viewport()->rect().center());
    painter.drawText(textRect, Qt::AlignCenter, textToDisplay);
}

void AideTableView::setPlaceHolderText(QString text)
{
    placeHolderText = std::move(text);
}
