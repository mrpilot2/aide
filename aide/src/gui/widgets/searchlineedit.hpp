
#ifndef AIDE_SEARCH_LINE_EDIT_HPP
#define AIDE_SEARCH_LINE_EDIT_HPP

#include <QColor>
#include <QLineEdit>
#include <QList>
#include <QRect>
#include <QSet>
#include <QString>

class QPaintEvent;

namespace aide::widgets
{
    class SearchLineEdit : public QLineEdit
    {
    public:
        using QLineEdit::QLineEdit;

        void setTags(QList<QString> tags);

    protected:
        void paintEvent(QPaintEvent* event) override;

    private:
        QList<QRect> calculateHighlightRects();

        QSet<QString> m_tags;

        QColor m_highlightColor{0, 96, 100, 150};
    };

} // namespace aide::widgets

#endif // AIDE_SEARCH_LINE_EDIT_HPP
