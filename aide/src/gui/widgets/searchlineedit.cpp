
#include "searchlineedit.hpp"

#include <QFontMetrics>
#include <QPaintEvent>
#include <QPainter>
#include <QPainterPath>

using aide::widgets::SearchLineEdit;

void SearchLineEdit::setTags(QList<QString> tags)
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
    m_tags = QSet<QString>(tags.begin(), tags.end());
#else
    m_tags = tags.toSet();
#endif
}

void SearchLineEdit::paintEvent(QPaintEvent* event)
{
    QLineEdit::paintEvent(event);

    QPainter painter(this);

    auto rects = calculateHighlightRects();

    for (auto& rect : rects) {
        QPainterPath path;
        path.addRoundedRect(rect, ROUNDED_RECT_RADIUS, ROUNDED_RECT_RADIUS);
        painter.fillPath(path, m_highlightColor);
    }
}

QList<QRect> SearchLineEdit::calculateHighlightRects()
{
    QList<QRect> rects;

    for (const auto& tag : m_tags) {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        int pos = 0;
#else
        qsizetype pos = 0;
#endif
        const auto currentText{this->text().toLower()};
        while (pos >= 0) {
            pos = currentText.indexOf(tag.toLower(), pos);

            if (pos >= 0) {
                auto fontMetrics(this->fontMetrics());
#if QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
                const auto startPos =
                    fontMetrics.horizontalAdvance(this->text().mid(0, pos));
                const auto endPos =
                    startPos + fontMetrics.horizontalAdvance(
                                   this->text().mid(pos, tag.length()));
#else
                const auto startPos =
                    fontMetrics.width(this->text().mid(0, pos));
                const auto endPos =
                    startPos +
                    fontMetrics.width(this->text().mid(pos, tag.length()));
#endif
                const QRect tagsRect{QPoint(this->rect().left() + 2 + startPos,
                                            this->rect().top() + 2),
                                     QPoint(this->rect().left() + 2 + endPos,
                                            this->rect().bottom() - 2)};

                rects.append(tagsRect);
                pos += tag.length();
            }
        }
    }

    return rects;
}
