#ifndef AIDE_SETTINGS_PAGE_FILTER_PROXY_MODEL_HPP
#define AIDE_SETTINGS_PAGE_FILTER_PROXY_MODEL_HPP

#include <QSortFilterProxyModel>
#include <QString>

class QModelIndex;
class QObject;

namespace aide::core
{
    /**
     * @brief Filters the settings page tree by a search pattern.
     *
     * The proxy filters the underlying SettingsPageGroupTreeModel by the
     * display name of the pages (case-insensitive). Recursive filtering keeps
     * parent nodes visible whenever one of their descendants matches, so the
     * tree structure stays intact and the user keeps the context of where a
     * matching page lives in the hierarchy.
     */
    class SettingsPageFilterProxyModel : public QSortFilterProxyModel
    {
        Q_OBJECT

    public:
        explicit SettingsPageFilterProxyModel(QObject* parent = nullptr);

        /**
         * @brief Sets the search pattern used to filter the tree.
         *
         * An empty pattern restores the full tree.
         */
        void setSearchPattern(const QString& pattern);

    protected:
        /**
         * @brief Accepts a row when its display name or page content matches.
         *
         * A row is kept when its display name contains the pattern or, for
         * rows that correspond to a settings page, when the page's content
         * matches the pattern. Recursive filtering keeps ancestors visible.
         */
        [[nodiscard]] bool filterAcceptsRow(
            int sourceRow, const QModelIndex& sourceParent) const override;

    private:
        QString m_searchPattern;
    };
} // namespace aide::core

#endif // AIDE_SETTINGS_PAGE_FILTER_PROXY_MODEL_HPP
