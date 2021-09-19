#ifndef AIDE_TREE_ITEM_HPP
#define AIDE_TREE_ITEM_HPP

#include <memory>
#include <vector>

#include <QVariant>

namespace aide::core
{
    class TreeItem;

    using TreeItemPtr = std::shared_ptr<TreeItem>;

    class TreeItem
    {
    public:
        explicit TreeItem(std::vector<QVariant> data,
                          TreeItemPtr parent = nullptr);

        void appendChild(TreeItemPtr child);

        void setHiddenUserData(QVariant data);

        [[nodiscard]] QVariant getHiddenUserData() const;

        TreeItemPtr child(size_t row);
        [[nodiscard]] size_t childCount() const;
        [[nodiscard]] size_t columnCount() const;
        [[nodiscard]] QVariant data(size_t column) const;
        bool setData(size_t column, const QVariant& data);
        [[nodiscard]] int64_t row() const;
        [[nodiscard]] TreeItemPtr parent() const;

    private:
        std::vector<TreeItemPtr> childItems;
        std::vector<QVariant> itemData;
        TreeItemPtr parentItem;

        QVariant hiddenUserData;
    };
} // namespace aide::core

#endif // AIDE_TREE_ITEM_HPP
