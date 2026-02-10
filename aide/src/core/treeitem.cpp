#include "treeitem.hpp"

#include <utility>

using aide::core::TreeItem;
using aide::core::TreeItemPtr;

TreeItem::TreeItem(std::vector<QVariant> data, const TreeItemPtr& parent)
    : m_itemData(std::move(data))
    , m_parentItem(parent)
{}

void TreeItem::appendChild(TreeItemPtr child)
{
    m_childItems.push_back(std::move(child));
}

void TreeItem::setHiddenUserData(QVariant data)
{
    m_hiddenUserData = std::move(data);
}

QVariant TreeItem::getHiddenUserData() const
{
    return m_hiddenUserData;
}

TreeItemPtr TreeItem::child(const size_t row)
{
    if (row >= m_childItems.size()) { return nullptr; }
    return m_childItems.at(row);
}

size_t TreeItem::childCount() const
{
    return m_childItems.size();
}

size_t TreeItem::columnCount() const
{
    return m_itemData.size();
}

QVariant TreeItem::data(const size_t column) const
{
    if (column >= m_itemData.size()) { return {}; }
    return m_itemData.at(column);
}

bool TreeItem::setData(const size_t column, const QVariant& data)
{
    if (column > m_itemData.size() - 1) { return false; }

    m_itemData.at(column) = data;

    return true;
}

int64_t TreeItem::row() const
{
    if (const auto parentItem = m_parentItem.lock(); parentItem != nullptr) {
        const auto childs{parentItem->m_childItems};

        return std::distance(
            childs.begin(),
            std::ranges::find_if(childs, [this](const auto& child) {
                return child.get() == this;
            }));
    }
    return 0;
}

std::weak_ptr<TreeItem> TreeItem::parent() const
{
    return m_parentItem;
}
