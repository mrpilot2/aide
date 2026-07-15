
#include "searchlineedit.hpp"

#include <QAction>
#include <QCompleter>
#include <QFontMetrics>
#include <QIcon>
#include <QPaintEvent>
#include <QPainter>
#include <QPainterPath>
#include <QStringListModel>

using aide::widgets::SearchLineEdit;

SearchLineEdit::SearchLineEdit(QWidget* parent)
    : QLineEdit(parent)
    , m_historyCompleter(new QCompleter(this))
    , m_historyModel(new QStringListModel(this))
{
    m_historyCompleter->setModel(m_historyModel);
    m_historyCompleter->setWidget(this);
    // The completer is driven exclusively by showHistory(); it is never set as
    // the line edit's completer, so typing can never auto-open the dropdown.
    m_historyCompleter->setCompletionMode(
        QCompleter::UnfilteredPopupCompletion);
    m_historyCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    connect(m_historyCompleter,
            qOverload<const QString&>(&QCompleter::activated), this,
            &SearchLineEdit::triggerSearch);

    m_historyAction =
        addAction(QIcon::fromTheme("edit-find"), QLineEdit::LeadingPosition);
    m_historyAction->setObjectName("searchHistoryAction");
    m_historyAction->setToolTip(tr("Show recent searches"));
    connect(m_historyAction, &QAction::triggered, this,
            &SearchLineEdit::showHistory);
}

void SearchLineEdit::setHistory(const QStringList& entries)
{
    m_historyModel->setStringList(entries);
}

QAction* SearchLineEdit::historyAction() const
{
    return m_historyAction;
}

QCompleter* SearchLineEdit::historyCompleter() const
{
    return m_historyCompleter;
}

void SearchLineEdit::showHistory()
{
    if (m_historyModel->stringList().isEmpty()) { return; }

    m_historyCompleter->setCompletionPrefix(QString());
    m_historyCompleter->complete();
}

void SearchLineEdit::triggerSearch(const QString& pattern)
{
    setText(pattern);
    // Behave exactly like the user pressing Enter so the dialog commits the
    // pattern to history and re-runs the search through the same signal path.
    emit returnPressed();
}

void SearchLineEdit::setTags(QList<QString> tags)
{
    m_tags = QSet(tags.begin(), tags.end());
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
        qsizetype pos = 0;
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
