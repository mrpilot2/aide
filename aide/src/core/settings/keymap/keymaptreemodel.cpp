
#include "keymaptreemodel.hpp"

#include <utility>

using aide::Action;
using aide::core::KeyMapTreeModel;
using aide::core::TreeItemPtr;

aide::core::KeyMapTreeModel::KeyMapTreeModel(
    ActionRegistryInterfacePtr registry, QObject* parent)
    : TreeModel(parent, std::make_shared<TreeItem>(
                            std::vector<QVariant>({{"Action", "Shortcuts"}})))
    , actionRegistry{std::move(registry)}
{
    setupModelData(actionRegistry);
}

void KeyMapTreeModel::setupModelData(
    const aide::ActionRegistryInterfacePtr& registry)
{
    for (const auto& action : registry->actions()) {
        TreeItemPtr current = rootItem;
        auto completeId     = action.first;

        for (auto iterator = completeId.begin(); iterator != completeId.end();
             ++iterator) {
            if (auto item = existingTreeItemForId(current, *iterator)) {
                current = std::move(*item);
                continue;
            }

            auto currentSubGroup =
                HierarchicalId(completeId.begin(), iterator + 1);

            auto keySequenceStringList =
                currentSubGroup == completeId
                    ? QKeySequence::listToString(
                          action.second.defaultKeySequences)
                    : "";

            auto child = std::make_shared<TreeItem>(
                std::vector<QVariant>({*iterator, keySequenceStringList}),
                current);

            child->setHiddenUserData(
                QString::fromStdString(currentSubGroup.name()));

            current->appendChild(child);
            current = std::move(child);
        }
    }
}

std::optional<TreeItemPtr> aide::core::KeyMapTreeModel::existingTreeItemForId(
    const aide::core::TreeItemPtr& current, const char* const& id)
{
    for (size_t i = 0; i < current->childCount(); ++i) {
        if (current->child(i)->data(0) == id) { return current->child(i); }
    }
    return {};
}

QVariant aide::core::KeyMapTreeModel::headerData(int section,
                                                 Qt::Orientation orientation,
                                                 int role) const
{
    if (role == Qt::TextAlignmentRole && section == 1) {
        return Qt::AlignRight;
    }
    return TreeModel::headerData(section, orientation, role);
}

QVariant KeyMapTreeModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid()) { return QVariant(); }

    if (role == Qt::TextAlignmentRole && index.column() == 1) {
        return Qt::AlignRight;
    }

    if (role == Qt::ToolTipRole && index.column() == 0) {
        if (auto action = findCorrespondingAction(index)) {
            return QString::fromStdString(action->description);
        }
    }

    if (role == Qt::DisplayRole) {
        auto* item = static_cast<TreeItem*>(index.internalPointer());

        return item->data(static_cast<size_t>(index.column()));
    }

    return QVariant();
}

std::optional<Action> KeyMapTreeModel::findCorrespondingAction(
    const QModelIndex& selectedIndex) const
{
    auto* item = static_cast<TreeItem*>(selectedIndex.internalPointer());

    auto completeGroupName{item->getHiddenUserData().toString().toStdString()};

    auto actions = actionRegistry->actions();

    if (auto it = std::find_if(actions.begin(), actions.end(),
                               [completeGroupName](const auto& action) {
                                   return action.first.name() ==
                                          completeGroupName;
                               });
        it != actions.end()) {
        return it->second;
    }
    return {};
}
