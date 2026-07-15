
#include "keymaptreemodel.hpp"

#include <memory>
#include <utility>

#include <QAction>
#include <QColor>
#include <QMenu>

#include <aide/hierarchicalid.hpp>

using aide::Action;
using aide::core::KeyMapTreeModel;
using aide::core::TreeItemPtr;

namespace
{
    // Qt exposes no public API to strip a menu/action mnemonic marker, so a
    // leading '&' before a character is a mnemonic and is removed, while a
    // literal ampersand is escaped as '&&' in Qt's convention and collapses
    // to a single '&'. Anything else, including a trailing ellipsis, is left
    // untouched.
    QString stripMnemonic(const QString& text)
    {
        QString result;
        result.reserve(text.size());

        for (qsizetype i = 0; i < text.size(); ++i) {
            if (text.at(i) != QLatin1Char('&')) {
                result += text.at(i);
                continue;
            }

            if (const auto next = i + 1;
                next < text.size() && text.at(next) == QLatin1Char('&')) {
                result += QLatin1Char('&');
                i = next;
            }
        }
        return result;
    }
} // namespace

KeyMapTreeModel::KeyMapTreeModel(ActionRegistryInterfacePtr registry,
                                 QObject* parent)
    : TreeModel(parent, std::make_shared<TreeItem>(
                            std::vector<QVariant>({{"Action", "Shortcuts"}})))
    , actionRegistry{std::move(registry)}
{
    setupModelData();
}

void KeyMapTreeModel::setupModelData()
{
    for (const auto& [key, action] : actionRegistry->actions()) {
        TreeItemPtr current    = m_rootItem;
        const auto& completeId = key;

        for (auto iterator = completeId.begin(); iterator != completeId.end();
             ++iterator) {
            if (auto item = existingTreeItemForId(current, *iterator)) {
                current = std::move(*item);
                auto currentSubGroup =
                    HierarchicalId(completeId.begin(), iterator + 1);

                auto keySequenceStringList =
                    currentSubGroup == completeId
                        ? QKeySequence::listToString(
                              action.getActiveKeySequences())
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
                    ? QKeySequence::listToString(action.getActiveKeySequences())
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

std::optional<TreeItemPtr> KeyMapTreeModel::existingTreeItemForId(
    const TreeItemPtr& current, const char* const& id)
{
    for (size_t i = 0; i < current->childCount(); ++i) {
        if (current->child(i)->data(0) == id) { return current->child(i); }
    }
    return {};
}

QVariant KeyMapTreeModel::headerData(const int section,
                                     const Qt::Orientation orientation,
                                     const int role) const
{
    if (role == Qt::TextAlignmentRole && section == 1) {
        return Qt::AlignRight;
    }
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        if (section == 0) { return tr("Action"); }
        if (section == 1) { return tr("Shortcuts"); }
    }
    return TreeModel::headerData(section, orientation, role);
}

QVariant KeyMapTreeModel::data(const QModelIndex& index, const int role) const
{
    if (!index.isValid()) { return {}; }

    if (role == Qt::TextAlignmentRole && index.column() == 1) {
        return Qt::AlignRight;
    }

    if (role == Qt::ToolTipRole && index.column() == 0) {
        if (const auto action = findCorrespondingAction(index)) {
            return QString::fromStdString(action->description);
        }
    }

    if (role == Qt::DecorationRole && index.column() == 0) {
        const auto action = findCorrespondingAction(index);
        if (!action) { return QIcon::fromTheme("folder"); }
        if (const auto qaction = action->action.lock(); qaction != nullptr) {
            return qaction->icon();
        }
        return QIcon::fromTheme("folder");
    }

    if (role == Qt::DisplayRole) {
        const auto* item = static_cast<TreeItem*>(index.internalPointer());

        if (index.column() == 0) {
            if (const auto label = translatedLabel(index)) { return *label; }
        }

        return item->data(static_cast<size_t>(index.column()));
    }

    if (role == Qt::ForegroundRole) {
        return isAnyUserSelectedKeySequencesInGroup(index) ? QColor(Qt::blue)
                                                           : QVariant();
    }

    return {};
}

bool KeyMapTreeModel::setData(const QModelIndex& index, const QVariant& value,
                              const int role)
{
    if (role == Qt::DisplayRole) {
        auto* item = static_cast<TreeItem*>(index.internalPointer());

        const auto res =
            item->setData(static_cast<size_t>(index.column()), value);

        emit dataChanged(index, index, {Qt::DisplayRole, Qt::ForegroundRole});

        return res;
    }
    return QAbstractItemModel::setData(index, value, role);
}

std::optional<Action> KeyMapTreeModel::findCorrespondingAction(
    const QModelIndex& selectedIndex) const
{
    const auto* item = static_cast<TreeItem*>(selectedIndex.internalPointer());

    auto completeGroupName{item->getHiddenUserData().toString().toStdString()};

    auto actions = actionRegistry->actions();

    if (const auto it = std::ranges::find_if(
            actions,
            [&completeGroupName](const auto& action) {
                return action.first.name() == completeGroupName;
            });
        it != actions.end()) {
        return it->second;
    }
    return {};
}

std::optional<QString> KeyMapTreeModel::translatedLabel(
    const QModelIndex& index) const
{
    if (const auto action = findCorrespondingAction(index)) {
        if (const auto qaction = action->action.lock();
            qaction != nullptr && !qaction->text().isEmpty()) {
            return stripMnemonic(qaction->text());
        }
        return {};
    }

    if (const auto menu = findCorrespondingMenu(index);
        menu && *menu != nullptr && !(*menu)->title().isEmpty()) {
        return stripMnemonic((*menu)->title());
    }
    return {};
}

std::optional<QMenu*> KeyMapTreeModel::findCorrespondingMenu(
    const QModelIndex& selectedIndex) const
{
    const auto* item = static_cast<TreeItem*>(selectedIndex.internalPointer());

    auto completeGroupName{item->getHiddenUserData().toString().toStdString()};

    const auto& menus = actionRegistry->menus();

    if (const auto it = std::ranges::find_if(
            menus,
            [&completeGroupName](const auto& menu) {
                return menu.first.name() == completeGroupName;
            });
        it != menus.end()) {
        return it->second->menu();
    }
    return {};
}

void KeyMapTreeModel::notifyShortcutsChangedForId(
    const HierarchicalId& id, const QList<QKeySequence>& shortcuts)
{
    if (auto item = findItemForActionId(id)) {
        item.value()->setData(1, QKeySequence::listToString(shortcuts));
        emit dataChanged(QModelIndex(), QModelIndex(),
                         {Qt::DisplayRole, Qt::ForegroundRole});
    }
}

std::optional<TreeItemPtr> KeyMapTreeModel::findItemForActionId(
    const HierarchicalId& id) const
{
    return recursivelyFindItemForActionId(m_rootItem, id);
}

std::optional<TreeItemPtr> KeyMapTreeModel::recursivelyFindItemForActionId(
    TreeItemPtr item, const HierarchicalId& id)
{
    if (const auto completeGroupName{
            item->getHiddenUserData().toString().toStdString()};
        completeGroupName == id.name()) {
        return item;
    }

    for (size_t i = 0; i < item->childCount(); ++i) {
        if (auto res = recursivelyFindItemForActionId(item->child(i), id);
            res.has_value()) {
            return res;
        }
    }
    return {};
}

bool KeyMapTreeModel::isAnyUserSelectedKeySequencesInGroup(
    const QModelIndex& index) const
{
    if (!index.isValid()) { return false; }

    if (const auto action = findCorrespondingAction(index)) {
        const auto* item = static_cast<TreeItem*>(index.internalPointer());
        const auto currentKeySequences =
            QKeySequence::listFromString(item->data(1).toString());

        return !Action::areKeySequencesTheSame(action->defaultKeySequences,
                                               currentKeySequences);
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
