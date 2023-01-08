
#include "searchlineedit.hpp"

#include <QFontMetrics>
#include <QPaintEvent>
#include <QPainter>
#include <QPainterPath>

using aide::widgets::SearchLineEdit;

void SearchLineEdit::setTags(QList<QString> tags)
{
    m_tags = QSet<QString>(tags.begin(), tags.end());
}

void SearchLineEdit::paintEvent(QPaintEvent* event)
{
    QLineEdit::paintEvent(event);

    QPainter painter(this);

    auto rects = calculateHighlightRects();

    for (auto& rect : rects) {
        QPainterPath path;
        path.addRoundedRect(rect, 5, 5);
        painter.fillPath(path, m_highlightColor);
    }
}

QList<QRect> SearchLineEdit::calculateHighlightRects()
{
    QList<QRect> rects;

    for (const auto& tag : m_tags) {
#if QT_VERSION < QT_VERSION_CHECK(6, 0,0)
        int pos = 0;
#else
        qsizetype pos = 0;
#endif
        const auto currentText{this->text().toLower()};
        while (pos >= 0) {
            pos = currentText.indexOf(tag.toLower(), pos);

            if (pos >= 0) {
                auto fontMetrics(this->fontMetrics());
                const auto startPos =
                    fontMetrics.horizontalAdvance(this->text().mid(0, pos));
                const auto endPos =
                    startPos + fontMetrics.horizontalAdvance(
                                   this->text().mid(pos, tag.length()));
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
