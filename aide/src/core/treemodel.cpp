
#include "treemodel.hpp"

#include <utility>

using aide::core::TreeModel;

TreeModel::TreeModel(QObject* parent, TreeItemPtr rootItem)
    : QAbstractItemModel(parent)
    , m_rootItem(std::move(rootItem))
{}

// NOLINTNEXTLINE
QModelIndex TreeModel::index(const int row, const int column,
                             const QModelIndex& parent) const
{
    if (!hasIndex(row, column, parent)) { return {}; }

    TreeItem* parentItem{
        !parent.isValid() ? m_rootItem.get()
                          : static_cast<TreeItem*>(parent.internalPointer())};

    if (const auto childItem = parentItem->child(static_cast<size_t>(row));
        childItem != nullptr) {
        return createIndex(row, column, childItem.get());
    }

    return {};
}

QVariant TreeModel::headerData(const int section,
                               const Qt::Orientation orientation,
                               const int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        return m_rootItem->data(static_cast<size_t>(section));
    }

    return {};
}

QModelIndex TreeModel::parent(const QModelIndex& index) const
{
    if (!index.isValid()) { return {}; }

    auto const* childItem = static_cast<TreeItem*>(index.internalPointer());

    const TreeItemPtr parentItem = childItem->parent().lock();

    if (parentItem == nullptr || parentItem == m_rootItem) { return {}; }

    return QAbstractItemModel::createIndex(static_cast<int>(parentItem->row()),
                                           0, parentItem.get());
}

// NOLINTNEXTLINE
int TreeModel::rowCount(const QModelIndex& parent) const
{
    if (parent.column() > 0) { return 0; }

    TreeItem const* parentItem{
        !parent.isValid() ? m_rootItem.get()
                          : static_cast<TreeItem*>(parent.internalPointer())};

    return static_cast<int>(parentItem->childCount());
}

// NOLINTNEXTLINE
int TreeModel::columnCount(const QModelIndex& parent) const
{
    return !parent.isValid() ? static_cast<int>(m_rootItem->columnCount())
                             : static_cast<int>(static_cast<TreeItem*>(
                                                    parent.internalPointer())
                                                    ->columnCount());
}
