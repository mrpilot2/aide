
#include "keymapcontroller.hpp"

#include <utility>

#include <QKeySequence>

#include "treeitem.hpp"

using aide::HierarchicalId;
using aide::core::KeymapController;
using aide::core::KeyMapTreeModel;
using aide::core::TreeItem;

KeymapController::KeymapController(const ActionRegistryInterfacePtr& registry,
                                   QObject* parent)
    : QObject(parent)
    , m_registry{registry}
    , m_pending{registry}
    , m_model{std::make_shared<KeyMapTreeModel>(registry)}
{
    connect(m_model.get(), &QAbstractItemModel::dataChanged, this,
            [this](const QModelIndex& topLeft,
                   const QModelIndex& /*bottomRight*/, const auto& roles) {
                if (!topLeft.isValid()) { return; }
                if (!roles.contains(Qt::DisplayRole)) { return; }
                if (topLeft.column() != 1) { return; }

                const auto* item =
                    static_cast<const TreeItem*>(topLeft.internalPointer());
                const auto idStr =
                    item->getHiddenUserData().toString().toStdString();
                if (idStr.empty()) { return; }

                for (const auto& [id, action] : m_registry->actions()) {
                    if (id.name() == idStr) {
                        m_pending.setPendingShortcuts(
                            id, QKeySequence::listFromString(
                                    item->data(1).toString()));
                        return;
                    }
                }
            });
}

bool KeymapController::isModified() const
{
    return m_pending.isAnyDirty();
}

void KeymapController::apply()
{
    m_pending.commitToRegistry();
    m_model->setupModelData();
}

void KeymapController::reset()
{
    m_pending.reset();
    m_model->setupModelData();
}

const std::shared_ptr<KeyMapTreeModel>& KeymapController::treeModel() const
{
    return m_model;
}

void KeymapController::setPendingShortcuts(const HierarchicalId& id,
                                           QList<QKeySequence> shortcuts)
{
    m_pending.setPendingShortcuts(id, std::move(shortcuts));
    syncPendingToModel(id);
}

void KeymapController::revertAction(const HierarchicalId& id)
{
    m_pending.revertAction(id);
    syncPendingToModel(id);
}

QList<QKeySequence> KeymapController::pendingShortcuts(
    const HierarchicalId& id) const
{
    return m_pending.pendingShortcuts(id);
}

void KeymapController::syncPendingToModel(const HierarchicalId& id)
{
    m_model->notifyShortcutsChangedForId(id, m_pending.pendingShortcuts(id));
}
