#ifndef AIDE_SETTINGS_PAGE_GROUP_TREE_MODEL_HPP
#define AIDE_SETTINGS_PAGE_GROUP_TREE_MODEL_HPP

#include <optional>

#include "settings/settingspage.hpp"
#include "treeitem.hpp"
#include "treemodel.hpp"

class QObject;

namespace aide::core
{
    class SettingsPageGroupTreeModel : public TreeModel
    {
        Q_OBJECT

    public:
        explicit SettingsPageGroupTreeModel(QObject* parent = nullptr);

        [[nodiscard]] QVariant data(const QModelIndex& index,
                                    int role) const override;

        [[nodiscard]] Qt::ItemFlags flags(
            const QModelIndex& index) const override;

        [[nodiscard]] static SettingsPagePtr findCorrespondingSettingsPage(
            const QModelIndex& selectedIndex);

        [[nodiscard]] QModelIndex recursivelyFindSelectedTreeItemIndex(
            const QString& groupName, const QModelIndex& parent) const;

    private:
        static void setupModelData(const TreeItemPtr& parent);

        static std::optional<TreeItemPtr> existingTreeItemForGroup(
            const TreeItemPtr& current, const char* group);
    };
} // namespace aide::core

#endif // AIDE_SETTINGS_PAGE_GROUP_TREE_MODEL_HPP
