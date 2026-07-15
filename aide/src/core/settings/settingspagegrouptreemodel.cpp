#include "settingspagegrouptreemodel.hpp"

#include <iterator>

#include <QColor>

#include "hierarchicalid.hpp"
#include "settings/settingspage.hpp"
#include "settings/settingspageregistry.hpp"
#include "treemodel.hpp"

using aide::core::SettingsPageGroupTreeModel;
using aide::core::SettingsPagePtr;
using aide::core::TreeItemPtr;

SettingsPageGroupTreeModel::SettingsPageGroupTreeModel(
    SettingsPageRegistry& registry, QObject* parent)
    : TreeModel(parent, std::make_shared<TreeItem>(
                            std::vector<QVariant>({{"Group"}}), nullptr))
    , m_registry(registry)
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
    const auto& pages = m_registry.settingsPages();

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
        const auto* item = static_cast<TreeItem*>(index.internalPointer());

        if (index.column() == 0) {
            if (const auto title = translatedGroupTitle(index)) {
                return *title;
            }
        }

        return item->data(static_cast<size_t>(index.column()));
    }
    if (role == Qt::ForegroundRole) {
        const auto page = findCorrespondingSettingsPage(index);

        return page != nullptr && page->isModified() ? QColor(Qt::blue)
                                                     : QVariant();
    }

    return {};
}

QVariant SettingsPageGroupTreeModel::headerData(
    const int section, const Qt::Orientation orientation, const int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal &&
        section == 0) {
        return tr("Group");
    }
    return TreeModel::headerData(section, orientation, role);
}

Qt::ItemFlags SettingsPageGroupTreeModel::flags(const QModelIndex& index) const
{
    if (!index.isValid()) { return Qt::NoItemFlags; }

    return QAbstractItemModel::flags(index);
}

SettingsPagePtr SettingsPageGroupTreeModel::findCorrespondingSettingsPage(
    const QModelIndex& selectedIndex) const
{
    const auto* item = static_cast<TreeItem*>(selectedIndex.internalPointer());

    auto completeGroupName{item->getHiddenUserData().toString().toStdString()};

    const auto& pages = m_registry.settingsPages();

    if (const auto it = std::ranges::find_if(
            pages,
            [&completeGroupName](const auto& page) {
                return page->group().name() == completeGroupName;
            });
        it != pages.end()) {
        return *it;
    }
    return nullptr;
}

qsizetype SettingsPageGroupTreeModel::depthOf(const TreeItem* item) const
{
    qsizetype depth = 0;
    for (auto parent = item->parent().lock();
         parent != nullptr && parent != m_rootItem;
         parent = parent->parent().lock()) {
        ++depth;
    }
    return depth;
}

std::optional<QString> SettingsPageGroupTreeModel::translatedGroupTitle(
    const QModelIndex& selectedIndex) const
{
    const auto* item = static_cast<TreeItem*>(selectedIndex.internalPointer());

    auto completeGroupName{item->getHiddenUserData().toString().toStdString()};
    const auto depth = depthOf(item);

    const auto& pages = m_registry.settingsPages();

    const auto it = std::ranges::find_if(pages, [&](const auto& page) {
        const auto& group = page->group();
        if (std::distance(group.begin(), group.end()) <= depth) {
            return false;
        }
        return HierarchicalId(group.begin(), group.begin() + depth + 1)
                   .name() == completeGroupName;
    });

    if (it == pages.end()) { return {}; }

    const auto titles = (*it)->groupTitles();
    if (titles.size() <= depth) { return {}; }

    return titles.at(depth);
}

QModelIndex SettingsPageGroupTreeModel::recursivelyFindSelectedTreeItemIndex(
    const QString& groupName, const QModelIndex& parent) const
{
    QModelIndex index;
    for (int i = 0; i < rowCount(parent); ++i) {
        index = this->index(i, 0, parent);

        if (const auto* item = static_cast<TreeItem*>(index.internalPointer());
            item->getHiddenUserData().toString() == groupName) {
            return index;
        }

        index = recursivelyFindSelectedTreeItemIndex(groupName, index);
        if (index.isValid()) { break; }
    }

    return index;
}
