
#include "treemodel.hpp"

#include <utility>

aide::core::TreeModel::TreeModel(QObject* parent, aide::core::TreeItemPtr root)
    : QAbstractItemModel(parent)
    , rootItem(std::move(root))
{}

// NOLINTNEXTLINE
QModelIndex aide::core::TreeModel::index(int row, int column,
                                         const QModelIndex& parent) const
{
    if (!hasIndex(row, column, parent)) { return QModelIndex(); }

    TreeItem* parentItem{
        !parent.isValid() ? rootItem.get()
                          : static_cast<TreeItem*>(parent.internalPointer())};

    auto childItem = parentItem->child(static_cast<size_t>(row));

    if (childItem != nullptr) {
        return createIndex(row, column, childItem.get());
    }

    return QModelIndex();
}

QVariant aide::core::TreeModel::headerData(int section,
                                           Qt::Orientation orientation,
                                           int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        return rootItem->data(static_cast<size_t>(section));
    }

    return QVariant();
}

QModelIndex aide::core::TreeModel::parent(const QModelIndex& index) const
{
    if (!index.isValid()) { return QModelIndex(); }

    auto* childItem        = static_cast<TreeItem*>(index.internalPointer());
    TreeItemPtr parentItem = childItem->parent().lock();

    if (parentItem == rootItem) { return QModelIndex(); }

    return QAbstractItemModel::createIndex(static_cast<int>(parentItem->row()),
                                           0, parentItem.get());
}

// NOLINTNEXTLINE
int aide::core::TreeModel::rowCount(const QModelIndex& parent) const
{
    if (parent.column() > 0) { return 0; }

    TreeItem* parentItem{
        !parent.isValid() ? rootItem.get()
                          : static_cast<TreeItem*>(parent.internalPointer())};

    return static_cast<int>(parentItem->childCount());
}

// NOLINTNEXTLINE
int aide::core::TreeModel::columnCount(const QModelIndex& parent) const
{
    return !parent.isValid() ? static_cast<int>(rootItem->columnCount())
                             : static_cast<int>(static_cast<TreeItem*>(
                                                    parent.internalPointer())
                                                    ->columnCount());
}
