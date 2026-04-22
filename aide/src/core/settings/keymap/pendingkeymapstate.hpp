
#ifndef AIDE_PENDING_KEYMAP_STATE_HPP
#define AIDE_PENDING_KEYMAP_STATE_HPP

#include <actionregistryinterface.hpp>
#include <map>

#include <QKeySequence>
#include <QList>

#include <aide/hierarchicalid.hpp>

namespace aide::core
{
    class PendingKeymapState
    {
    public:
        explicit PendingKeymapState(ActionRegistryInterfacePtr registry);

        void setPendingShortcuts(const HierarchicalId& id,
                                 QList<QKeySequence> shortcuts);
        void revertAction(const HierarchicalId& id);
        void reset();

        [[nodiscard]] QList<QKeySequence> pendingShortcuts(
            const HierarchicalId& id) const;
        [[nodiscard]] bool isDirty(const HierarchicalId& id) const;
        [[nodiscard]] bool isAnyDirty() const;

        void commitToRegistry();

    private:
        ActionRegistryInterfacePtr m_registry;
        std::map<HierarchicalId, QList<QKeySequence>> m_baseline;
        std::map<HierarchicalId, QList<QKeySequence>> m_pending;
    };
} // namespace aide::core

#endif // AIDE_PENDING_KEYMAP_STATE_HPP
