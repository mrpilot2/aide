
#ifndef AIDE_KEY_MAP_TREE_MODEL_HPP
#define AIDE_KEY_MAP_TREE_MODEL_HPP

#include <actionregistryinterface.hpp>
#include <optional>
#include <treeitem.hpp>
#include <treemodel.hpp>

namespace aide::core
{
    class KeyMapTreeModel : public TreeModel
    {
        Q_OBJECT

    public:
        explicit KeyMapTreeModel(ActionRegistryInterfacePtr registry,
                                 QObject* parent = nullptr);

        [[nodiscard]] QVariant data(const QModelIndex& index,
                                    int role) const override;

        bool setData(const QModelIndex& index, const QVariant& value,
                     int role) override;

        QVariant headerData(int section, Qt::Orientation orientation,
                            int role) const override;

        [[nodiscard]] std::optional<Action> findCorrespondingAction(
            const QModelIndex& selectedIndex) const;

        [[nodiscard]] std::optional<TreeItemPtr> findItemForActionId(
            HierarchicalId id);

        void setupModelData();

    private:
        bool isAnyUserSelectedKeySequencesInGroup(
            const QModelIndex& index) const;

        static std::optional<TreeItemPtr> existingTreeItemForId(
            const aide::core::TreeItemPtr& current, const char* const& id);

        ActionRegistryInterfacePtr actionRegistry;
        std::optional<TreeItemPtr> recursivelyFindItemForActionId(
            TreeItemPtr item, HierarchicalId id);
    };
} // namespace aide::core

#endif // AIDE_KEY_MAP_TREE_MODEL_HPP
