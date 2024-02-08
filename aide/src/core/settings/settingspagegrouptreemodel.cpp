#include "settingspagegrouptreemodel.hpp"

#include <QColor>

#include "hierarchicalid.hpp"
#include "settings/settingspage.hpp"
#include "settings/settingspageregistry.hpp"
#include "treemodel.hpp"

using aide::core::SettingsPageGroupTreeModel;
using aide::core::SettingsPagePtr;
using aide::core::TreeItemPtr;

SettingsPageGroupTreeModel::SettingsPageGroupTreeModel(QObject* parent)
    : TreeModel(parent, std::make_shared<TreeItem>(
                            std::vector<QVariant>({{"Group"}}), nullptr))
{
    setupModelData(m_rootItem);
}

std::optional<TreeItemPtr> SettingsPageGroupTreeModel::existingTreeItemForGroup(
    const TreeItemPtr& current, const char* group)
{
    for (size_t i = 0; i < current->childCount(); ++i) {
        if (current->child(i)->data(0) == group) { return current->child(i); }
    }
    return {};
}

void SettingsPageGroupTreeModel::setupModelData(const TreeItemPtr& parent)
{
    const auto& pages = SettingsPageRegistry::settingsPages();

    for (const auto& page : pages) {
        TreeItemPtr current = parent;
        auto groups         = page->group();

        for (auto iterator = groups.begin(); iterator != groups.end();
             ++iterator) {
            if (auto item = existingTreeItemForGroup(current, *iterator)) {
                current = std::move(*item);
                continue;
            }

            auto currentSubGroup = HierarchicalId(groups.begin(), iterator + 1);

            auto child = std::make_shared<TreeItem>(
                std::vector<QVariant>({*iterator}), current);
            child->setHiddenUserData(
                QString::fromStdString(currentSubGroup.name()));
            current->appendChild(child);
            current = std::move(child);
        }
    }
}

QVariant SettingsPageGroupTreeModel::data(const QModelIndex& index,
                                          const int role) const
{
    if (!index.isValid()) { return {}; }

    if (role == Qt::DisplayRole) {
        auto const* item = static_cast<TreeItem*>(index.internalPointer());

        return item->data(static_cast<size_t>(index.column()));
    }
    if (role == Qt::ForegroundRole) {
        const auto page = findCorrespondingSettingsPage(index);

        return page != nullptr && page->isModified() ? QColor(Qt::blue)
                                                     : QVariant();
    }

    return {};
}

Qt::ItemFlags SettingsPageGroupTreeModel::flags(const QModelIndex& index) const
{
    if (!index.isValid()) { return Qt::NoItemFlags; }

    return QAbstractItemModel::flags(index);
}

SettingsPagePtr SettingsPageGroupTreeModel::findCorrespondingSettingsPage(
    const QModelIndex& selectedIndex)
{
    auto const* item = static_cast<TreeItem*>(selectedIndex.internalPointer());

    auto completeGroupName{item->getHiddenUserData().toString().toStdString()};

    const auto& pages = SettingsPageRegistry::settingsPages();

    if (const auto it = std::find_if(pages.begin(), pages.end(),
                                     [&completeGroupName](const auto& page) {
                                         return page->group().name() ==
                                                completeGroupName;
                                     });
        it != pages.end()) {
        return *it;
    }
    return nullptr;
}

QModelIndex SettingsPageGroupTreeModel::recursivelyFindSelectedTreeItemIndex(
    const QString& groupName, const QModelIndex& parent) const
{
    QModelIndex index;
    for (int i = 0; i < rowCount(parent); ++i) {
        index = this->index(i, 0, parent);

        if (auto const* item = static_cast<TreeItem*>(index.internalPointer());
            item->getHiddenUserData().toString() == groupName) {
            return index;
        }

        index = recursivelyFindSelectedTreeItemIndex(groupName, index);
        if (index.isValid()) { break; }
    }

    return index;
}
