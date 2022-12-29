#ifndef AIDE_SEARCHLINEEDIT_HPP
#define AIDE_SEARCHLINEEDIT_HPP

#include <memory>

#include <QSortFilterProxyModel>
#include <QWidget>

namespace Ui
{
    class SearchLineEdit;
} // namespace Ui

namespace aide::widgets
{
    class MultiColumnSortFilterProxyModel;

    class SearchLineEdit : public QWidget
    {
        Q_OBJECT
    public:
        explicit SearchLineEdit(QWidget* parent);

        ~SearchLineEdit() override;

        void setSearchIcon(const QIcon& icon);

        void setSourceModel(QAbstractItemModel* model);

        QSortFilterProxyModel* getFilterModel();
    private slots:
        void textChanged(const QString& text);

    private:
        void filterEntries();

        std::unique_ptr<Ui::SearchLineEdit> m_ui;

        QTimer* m_typingTimer;
        QString m_currentFilterText;

        MultiColumnSortFilterProxyModel* m_filterModel;
    };
} // namespace aide::widgets

#endif // AIDE_SEARCHLINEEDIT_HPP
