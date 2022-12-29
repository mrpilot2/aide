#ifndef AIDE_SEARCHLINEEDIT_HPP
#define AIDE_SEARCHLINEEDIT_HPP

#include <memory>

#include <QSortFilterProxyModel>
#include <QWidget>

#include <aide/hierarchicalid.hpp>

namespace Ui
{
    class SearchLineEdit;
} // namespace Ui

namespace aide
{
    class HierarchicalId;
}

namespace aide::widgets
{
    class MultiColumnSortFilterProxyModel;

    class SearchLineEdit : public QWidget
    {
        Q_OBJECT
    public:
        explicit SearchLineEdit(const aide::HierarchicalId& id,
                                const QKeySequence& showHideShortcut,
                                QWidget* parent);

        ~SearchLineEdit() override;

    public:
        void setSearchIcon(const QIcon& icon);

        void setSourceModel(QAbstractItemModel* model);

        QSortFilterProxyModel* getFilterModel();

    private slots:
        void onUserRequestsToChangeVisibility(bool visible);

        void textChanged(const QString& text);

        void matchCaseStateChanged(bool state);
        void regexStateChanged(bool state);

    private:
        void filterEntries();

    private:
        std::unique_ptr<Ui::SearchLineEdit> m_ui;

        QTimer* m_typingTimer;
        QString m_currentFilterText;

        MultiColumnSortFilterProxyModel* m_filterModel;

        const aide::HierarchicalId m_visibilitySettingsKey;
        const aide::HierarchicalId m_matchCaseSettingsKey;
        const aide::HierarchicalId m_regexSettingsKey;

        bool m_aboutToClose{false};
    };
} // namespace aide::widgets

#endif // AIDE_SEARCHLINEEDIT_HPP
