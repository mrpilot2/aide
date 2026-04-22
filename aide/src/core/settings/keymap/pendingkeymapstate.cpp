
#include "pendingkeymapstate.hpp"

#include <aide/actionregistryinterface.hpp>

using aide::HierarchicalId;
using aide::core::PendingKeymapState;

PendingKeymapState::PendingKeymapState(ActionRegistryInterfacePtr registry)
    : m_registry{std::move(registry)}
{
    reset();
}

void PendingKeymapState::reset()
{
    m_baseline.clear();
    for (const auto& [id, action] : m_registry->actions()) {
        m_baseline[id] = action.getActiveKeySequences();
    }
    m_pending = m_baseline;
}

void PendingKeymapState::setPendingShortcuts(const HierarchicalId& id,
                                             QList<QKeySequence> shortcuts)
{
    m_pending[id] = std::move(shortcuts);
}

void PendingKeymapState::revertAction(const HierarchicalId& id)
{
    if (const auto it = m_baseline.find(id); it != m_baseline.end()) {
        m_pending[id] = it->second;
    }
}

QList<QKeySequence> PendingKeymapState::pendingShortcuts(
    const HierarchicalId& id) const
{
    if (const auto it = m_pending.find(id); it != m_pending.end()) {
        return it->second;
    }
    return {};
}

bool PendingKeymapState::isDirty(const HierarchicalId& id) const
{
    const auto pit = m_pending.find(id);
    const auto bit = m_baseline.find(id);
    if (pit == m_pending.end() || bit == m_baseline.end()) { return false; }
    return !Action::areKeySequencesTheSame(pit->second, bit->second);
}

bool PendingKeymapState::isAnyDirty() const
{
    return std::ranges::any_of(
        m_pending, [this](const auto& entry) { return isDirty(entry.first); });
}

void PendingKeymapState::commitToRegistry()
{
    for (const auto& [id, shortcuts] : m_pending) {
        if (isDirty(id)) {
            m_registry->modifyShortcutsForAction(id, shortcuts);
        }
    }
    reset();
}
