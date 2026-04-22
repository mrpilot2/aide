
#ifndef AIDE_KEYMAP_CONTROLLER_HPP
#define AIDE_KEYMAP_CONTROLLER_HPP

#include <actionregistryinterface.hpp>
#include <memory>

#include <QModelIndex>
#include <QObject>

#include "keymaptreemodel.hpp"
#include "pendingkeymapstate.hpp"

namespace aide::core
{
    class KeymapController : public QObject
    {
        Q_OBJECT

    public:
        explicit KeymapController(const ActionRegistryInterfacePtr& registry,
                                  QObject* parent = nullptr);

        [[nodiscard]] bool isModified() const;
        void apply();
        void reset();

        [[nodiscard]] const std::shared_ptr<KeyMapTreeModel>& treeModel() const;

        void setPendingShortcuts(const HierarchicalId& id,
                                 QList<QKeySequence> shortcuts);
        void revertAction(const HierarchicalId& id);
        [[nodiscard]] QList<QKeySequence> pendingShortcuts(
            const HierarchicalId& id) const;

    private:
        ActionRegistryInterfacePtr m_registry;
        PendingKeymapState m_pending;
        std::shared_ptr<KeyMapTreeModel> m_model;

        void syncPendingToModel(const HierarchicalId& id);
    };
} // namespace aide::core

#endif // AIDE_KEYMAP_CONTROLLER_HPP
