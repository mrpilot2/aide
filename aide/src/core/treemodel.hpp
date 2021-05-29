
#ifndef AIDE_TREE_MODEL_HPP
#define AIDE_TREE_MODEL_HPP

#include <QAbstractItemModel>
#include <QObject>

#include "treeitem.hpp"

namespace aide::core
{
    class TreeModel : public QAbstractItemModel
    {
        Q_OBJECT
    public:
        TreeModel(QObject* parent, TreeItemPtr rootItem);

        [[nodiscard]] QModelIndex index(
            int row, int column,
            const QModelIndex& parent = QModelIndex()) const override;

        [[nodiscard]] QVariant headerData(int section,
                                          Qt::Orientation orientation,
                                          int role) const override;

        [[nodiscard]] QModelIndex parent(
            const QModelIndex& index) const override;

        [[nodiscard]] int rowCount(
            const QModelIndex& parent = QModelIndex()) const override;

        [[nodiscard]] int columnCount(
            const QModelIndex& parent = QModelIndex()) const override;

    protected:
        TreeItemPtr rootItem;
    };

} // namespace aide::core

#endif // AIDE_TREE_MODEL_HPP
