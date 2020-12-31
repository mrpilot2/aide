#include "settingspagegrouptreemodel.hpp"

#include "hierarchicalid.hpp"
#include "settings/settingspage.hpp"
#include "settings/settingspageregistry.hpp"

using aide::core::SettingsPageGroupTreeModel;
using aide::core::SettingsPageRegistry;
using aide::core::TreeItemPtr;

SettingsPageGroupTreeModel::SettingsPageGroupTreeModel(QObject* parent)
    : QAbstractItemModel(parent)
    , rootItem(std::make_shared<TreeItem>(
          std::vector<QVariant>({{"Group", "Complete"}})))
{
    setupModelData(rootItem);
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
                std::vector<QVariant>({*iterator, QString::fromStdString(
                                                      currentSubGroup.name())}),
                current);
            current->appendChild(child);
            current = std::move(child);
        }
    }
}

QVariant aide::core::SettingsPageGroupTreeModel::headerData(
    int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole &&
        section == 0) {
        return rootItem->data(static_cast<size_t>(section));
    }

    return QVariant();
}

QVariant SettingsPageGroupTreeModel::data(const QModelIndex& index,
                                          int role) const
{
    if (!index.isValid()) { return QVariant(); }

    if (role != Qt::DisplayRole) { return QVariant(); }

    auto* item = static_cast<TreeItem*>(index.internalPointer());

    return item->data(static_cast<size_t>(index.column()));
}

Qt::ItemFlags SettingsPageGroupTreeModel::flags(const QModelIndex& index) const
{
    if (!index.isValid()) { return Qt::NoItemFlags; }

    return QAbstractItemModel::flags(index);
}

// NOLINTNEXTLINE
QModelIndex SettingsPageGroupTreeModel::index(int row, int column,
                                              const QModelIndex& parent) const
{
    if (!hasIndex(row, column, parent)) { return QModelIndex(); }

    TreeItem* parentItem{
        !parent.isValid() ? rootItem.get()
                          : static_cast<TreeItem*>(parent.internalPointer())};

    auto childItem = parentItem->child(static_cast<size_t>(row));

    if (childItem != nullptr) {
        return createIndex(row, column, childItem.get());
    }

    return QModelIndex();
}

QModelIndex SettingsPageGroupTreeModel::parent(const QModelIndex& index) const
{
    if (!index.isValid()) { return QModelIndex(); }

    auto* childItem        = static_cast<TreeItem*>(index.internalPointer());
    TreeItemPtr parentItem = childItem->parent();

    if (parentItem == rootItem) { return QModelIndex(); }

    return createIndex(static_cast<int>(parentItem->row()), 0,
                       parentItem.get());
}

// NOLINTNEXTLINE
int SettingsPageGroupTreeModel::rowCount(const QModelIndex& parent) const
{
    if (parent.column() > 0) { return 0; }

    TreeItem* parentItem{
        !parent.isValid() ? rootItem.get()
                          : static_cast<TreeItem*>(parent.internalPointer())};

    return static_cast<int>(parentItem->childCount());
}
// NOLINTNEXTLINE
int SettingsPageGroupTreeModel::columnCount(const QModelIndex& parent) const
{
    return !parent.isValid() ? static_cast<int>(rootItem->columnCount())
                             : static_cast<int>(static_cast<TreeItem*>(
                                                    parent.internalPointer())
                                                    ->columnCount());
}
