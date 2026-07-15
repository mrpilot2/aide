#ifndef AIDE_SETTINGS_PAGE_GROUP_TREE_MODEL_HPP
#define AIDE_SETTINGS_PAGE_GROUP_TREE_MODEL_HPP

#include <optional>

#include "settings/settingspage.hpp"
#include "settings/settingspageregistry.hpp"
#include "treeitem.hpp"
#include "treemodel.hpp"

class QObject;

namespace aide::core
{
    class SettingsPageGroupTreeModel : public TreeModel
    {
        Q_OBJECT

    public:
        explicit SettingsPageGroupTreeModel(SettingsPageRegistry& registry,
                                            QObject* parent = nullptr);

        [[nodiscard]] QVariant data(const QModelIndex& index,
                                    int role) const override;

        [[nodiscard]] QVariant headerData(int section,
                                          Qt::Orientation orientation,
                                          int role) const override;

        [[nodiscard]] Qt::ItemFlags flags(
            const QModelIndex& index) const override;

        [[nodiscard]] SettingsPagePtr findCorrespondingSettingsPage(
            const QModelIndex& selectedIndex) const;

        [[nodiscard]] QModelIndex recursivelyFindSelectedTreeItemIndex(
            const QString& groupName, const QModelIndex& parent) const;

    private:
        void setupModelData(const TreeItemPtr& parent);

        static std::optional<TreeItemPtr> existingTreeItemForGroup(
            const TreeItemPtr& current, const char* group);

        [[nodiscard]] std::optional<QString> translatedGroupTitle(
            const QModelIndex& selectedIndex) const;

        [[nodiscard]] qsizetype depthOf(const TreeItem* item) const;

        SettingsPageRegistry& m_registry;
    };
} // namespace aide::core

#endif // AIDE_SETTINGS_PAGE_GROUP_TREE_MODEL_HPP
