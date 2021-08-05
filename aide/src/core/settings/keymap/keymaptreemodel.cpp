
#include "keymaptreemodel.hpp"

#include <memory>
#include <utility>

#include <QColor>

using aide::Action;
using aide::core::KeyMapTreeModel;
using aide::core::TreeItemPtr;

aide::core::KeyMapTreeModel::KeyMapTreeModel(
    ActionRegistryInterfacePtr registry, QObject* parent)
    : TreeModel(parent, std::make_shared<TreeItem>(
                            std::vector<QVariant>({{"Action", "Shortcuts"}})))
    , actionRegistry{registry}
{
    setupModelData();
}

void KeyMapTreeModel::setupModelData()
{
    for (const auto& action : actionRegistry->actions()) {
        TreeItemPtr current = rootItem;
        auto completeId     = action.first;

        for (auto iterator = completeId.begin(); iterator != completeId.end();
             ++iterator) {
            if (auto item = existingTreeItemForId(current, *iterator)) {
                current = std::move(*item);
                auto currentSubGroup =
                    HierarchicalId(completeId.begin(), iterator + 1);

                auto keySequenceStringList =
                    currentSubGroup == completeId
                    ? QKeySequence::listToString(
                        action.second.getActiveKeySequences())
                        : QString();
                current->setData(1, keySequenceStringList);
                emit dataChanged(QModelIndex(), QModelIndex(),
                                 {Qt::DisplayRole, Qt::ForegroundRole});
                continue;
            }

            auto currentSubGroup =
                HierarchicalId(completeId.begin(), iterator + 1);

            auto keySequenceStringList =
                currentSubGroup == completeId
                    ? QKeySequence::listToString(
                          action.second.getActiveKeySequences())
                    : QString();

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

    if (role == Qt::ForegroundRole) {
        return isAnyUserSelectedKeySequencesInGroup(index) ? QColor(Qt::blue)
                                                           : QVariant();
    }

    return QVariant();
}

bool aide::core::KeyMapTreeModel::setData(const QModelIndex& index,
                                          const QVariant& value, int role)
{
    if (role == Qt::DisplayRole) {
        auto* item = static_cast<TreeItem*>(index.internalPointer());

        const auto res = item->setData(1, value);

        emit dataChanged(index, index, {Qt::DisplayRole, Qt::ForegroundRole});

        return res;
    }
    return QAbstractItemModel::setData(index, value, role);
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

std::optional<TreeItemPtr> aide::core::KeyMapTreeModel::findItemForActionId(
    aide::HierarchicalId id)
{
    return recursivelyFindItemForActionId(rootItem, id);
}

std::optional<TreeItemPtr>
aide::core::KeyMapTreeModel::recursivelyFindItemForActionId(
    TreeItemPtr item, aide::HierarchicalId id)
{
    auto completeGroupName{item->getHiddenUserData().toString().toStdString()};

    if (completeGroupName == id.name()) { return item; }

    for (size_t i = 0; i < item->childCount(); ++i) {
        auto res = recursivelyFindItemForActionId(item->child(i), id);
        if (res) { return res; }
    }
    return std::optional<TreeItemPtr>();
}

bool aide::core::KeyMapTreeModel::isAnyUserSelectedKeySequencesInGroup(
    const QModelIndex& index) const
{
    if (!index.isValid()) { return false; }

    if (auto action = findCorrespondingAction(index)) {
        auto* item = static_cast<TreeItem*>(index.internalPointer());
        auto currentKeySequences =
            QKeySequence::listFromString(item->data(1).toString());

        return (!action->areKeySequencesTheSame(action->defaultKeySequences,
                                                currentKeySequences));
    }

    bool found = false;

    for (int i = 0; i < rowCount(index); ++i) {
        auto childIndex = this->index(i, 0, index);

        if (!childIndex.isValid()) { continue; }

        found = isAnyUserSelectedKeySequencesInGroup(childIndex);

        if (found) { break; }
    }
    return found;
}
