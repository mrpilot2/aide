#ifndef AIDE_SEARCH_FILTER_WIDGET_HPP
#define AIDE_SEARCH_FILTER_WIDGET_HPP

#include <memory>

#include <QAbstractItemDelegate>
#include <QSortFilterProxyModel>
#include <QWidget>

#include <aide/hierarchicalid.hpp>

namespace Ui
{
    class SearchFilterWidget;
} // namespace Ui

namespace aide
{
    class HierarchicalId;
} // namespace aide

class QAbstractItemDelegate;

namespace aide::widgets
{
    class MultiColumnSortFilterProxyModel;

    class SearchFilterWidget : public QWidget
    {
        Q_OBJECT
    public:
        explicit SearchFilterWidget(const aide::HierarchicalId& id,
                                    const QKeySequence& showHideShortcut,
                                    QWidget* parent);

        ~SearchFilterWidget() override;

        void setSearchIcon(const QIcon& icon);

        void setSourceModel(QAbstractItemModel* model);

        QSortFilterProxyModel* getFilterModel();

        QAbstractItemDelegate* getItemDelegate();

    private slots:
        void onUserRequestsToChangeVisibility(bool visible);

        void textChanged(const QString& text);

        void matchCaseStateChanged(bool state);
        void regexStateChanged(bool state);

        void filterEntries();

    private:
        std::unique_ptr<Ui::SearchFilterWidget> m_ui;

        QTimer* m_typingTimer;
        QString m_currentFilterText;

        MultiColumnSortFilterProxyModel* m_filterModel;

        const aide::HierarchicalId m_visibilitySettingsKey;
        const aide::HierarchicalId m_matchCaseSettingsKey;
        const aide::HierarchicalId m_regexSettingsKey;

        bool m_aboutToClose{false};
    };
} // namespace aide::widgets

#endif // AIDE_SEARCH_FILTER_WIDGET_HPP
