#ifndef AIDE_SETTINGS_PAGE_GROUP_TREE_MODEL_HPP
#define AIDE_SETTINGS_PAGE_GROUP_TREE_MODEL_HPP

#include <optional>

#include <QAbstractItemModel>
#include <QObject>

#include "treeitem.hpp"

namespace aide::core
{
    class SettingsPageGroupTreeModel : public QAbstractItemModel
    {
        Q_OBJECT

    public:
        explicit SettingsPageGroupTreeModel(QObject* parent = nullptr);

        [[nodiscard]] QVariant headerData(int section,
                                          Qt::Orientation orientation,
                                          int role) const override;

        [[nodiscard]] QVariant data(const QModelIndex& index,
                                    int role) const override;

        [[nodiscard]] Qt::ItemFlags flags(
            const QModelIndex& index) const override;

        [[nodiscard]] QModelIndex index(
            int row, int column,
            const QModelIndex& parent = QModelIndex()) const override;

        [[nodiscard]] QModelIndex parent(
            const QModelIndex& index) const override;

        [[nodiscard]] int rowCount(
            const QModelIndex& parent = QModelIndex()) const override;

        [[nodiscard]] int columnCount(
            const QModelIndex& parent = QModelIndex()) const override;

    private:
        static void setupModelData(const TreeItemPtr& parent);

        static std::optional<TreeItemPtr> existingTreeItemForGroup(
            const TreeItemPtr& current, const char* group);

        TreeItemPtr rootItem;
    };
} // namespace aide::core

#endif // AIDE_SETTINGS_PAGE_GROUP_TREE_MODEL_HPP
