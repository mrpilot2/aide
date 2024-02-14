#ifndef AIDE_SEARCH_FILTER_WIDGET_HPP
#define AIDE_SEARCH_FILTER_WIDGET_HPP

#include <memory>

#include <QAbstractItemDelegate>
#include <QAction>
#include <QSortFilterProxyModel>
#include <QWidget>

#include <aide/hierarchicalid.hpp>

static constexpr auto VISIBILITY_TOGGLE_DELAY = 200;
static constexpr auto TYPING_FILTER_DELAY     = 300;
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
        explicit SearchFilterWidget(const HierarchicalId& id,
                                    const QKeySequence& showHideShortcut,
                                    QWidget* parent);

        ~SearchFilterWidget() override;

        void setSearchIcon(const QIcon& icon) const;

        void setSourceModel(QAbstractItemModel* model) const;

        QSortFilterProxyModel* getFilterModel() const;

        QAbstractItemDelegate* getItemDelegate();

    private slots:
        void onUserRequestsToChangeVisibility(bool visible);

        void textChanged(const QString& text);

        void matchCaseStateChanged(bool state) const;
        void regexStateChanged(bool state) const;

        void filterEntries() const;

    private:
        std::unique_ptr<Ui::SearchFilterWidget> m_ui;

        QTimer* m_typingTimer;
        QString m_currentFilterText;

        MultiColumnSortFilterProxyModel* m_filterModel;

        QAction m_showHideAction;
        const HierarchicalId m_visibilitySettingsKey;
        const HierarchicalId m_matchCaseSettingsKey;
        const HierarchicalId m_regexSettingsKey;

        bool m_aboutToClose{false};
    };
} // namespace aide::widgets

#endif // AIDE_SEARCH_FILTER_WIDGET_HPP
