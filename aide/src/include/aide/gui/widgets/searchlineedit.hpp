
#ifndef AIDE_SEARCH_LINE_EDIT_HPP
#define AIDE_SEARCH_LINE_EDIT_HPP

#include <QColor>
#include <QLineEdit>
#include <QList>
#include <QRect>
#include <QSet>
#include <QString>
#include <QStringList>

static constexpr const auto ROUNDED_RECT_RADIUS = 5;
class QPaintEvent;
class QAction;
class QCompleter;
class QStringListModel;

namespace aide::widgets
{
    class SearchLineEdit : public QLineEdit
    {
        Q_OBJECT
    public:
        explicit SearchLineEdit(QWidget* parent = nullptr);

        void setTags(QList<QString> tags);

        /**
         * @brief Replace the on-demand history dropdown entries.
         *
         * Entries are shown most-recent-first, exactly in the order given.
         */
        void setHistory(const QStringList& entries);

        [[nodiscard]] QAction* historyAction() const;

        [[nodiscard]] QCompleter* historyCompleter() const;

    public slots:
        /**
         * @brief Open the history dropdown. Never called while typing.
         */
        void showHistory();

        /**
         * @brief Set the text to @p pattern and re-run the search, behaving
         * exactly like the user typing @p pattern and pressing Enter.
         */
        void triggerSearch(const QString& pattern);

    protected:
        void paintEvent(QPaintEvent* event) override;

    private:
        QList<QRect> calculateHighlightRects();

        QSet<QString> m_tags;

        QColor m_highlightColor{0, 96, 100, 150};

        QAction* m_historyAction{nullptr};
        QCompleter* m_historyCompleter{nullptr};
        QStringListModel* m_historyModel{nullptr};
    };

} // namespace aide::widgets

#endif // AIDE_SEARCH_LINE_EDIT_HPP
