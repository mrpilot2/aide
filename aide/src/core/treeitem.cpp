#include "treeitem.hpp"

#include <utility>

using aide::core::TreeItem;
using aide::core::TreeItemPtr;

TreeItem::TreeItem(std::vector<QVariant> data, TreeItemPtr parent)
    : itemData(std::move(data))
    , parentItem(std::move(parent))
{}

void TreeItem::appendChild(TreeItemPtr child)
{
    childItems.push_back(std::move(child));
}

TreeItemPtr TreeItem::child(size_t row)
{
    if (row >= childItems.size()) { return nullptr; }
    return childItems.at(row);
}

size_t TreeItem::childCount() const
{
    return childItems.size();
}

size_t TreeItem::columnCount() const
{
    return itemData.size();
}

QVariant TreeItem::data(size_t column) const
{
    if (column >= itemData.size()) { return QVariant(); }
    return itemData.at(column);
}

int64_t TreeItem::row() const
{
    if (parentItem) {
        const auto childs{parentItem->childItems};

        return std::distance(
            childs.begin(),
            std::find_if(childs.begin(), childs.end(),
                         [this](auto child) { return child.get() == this; }));
    }
    return 0;
}

TreeItemPtr TreeItem::parent() const
{
    return parentItem;
}
