#include "actionregistry.hpp"

#include <aide/logger.hpp>

using aide::Action;
using aide::ActionRegistry;
using aide::HierarchicalId;

void ActionRegistry::registerAction(std::weak_ptr<QAction> action,
                                    const HierarchicalId& uniqueId)
{
    registerAction(action, uniqueId, "", {});
}

void ActionRegistry::registerAction(std::weak_ptr<QAction> action,
                                    const HierarchicalId& uniqueId,
                                    std::string description)
{
    registerAction(action, uniqueId, description, {});
}

void ActionRegistry::registerAction(
    std::weak_ptr<QAction> action, const HierarchicalId& uniqueId,
    const std::vector<QKeySequence>& defaultKeySequences)
{
    registerAction(action, uniqueId, "", defaultKeySequences);
}

void ActionRegistry::registerAction(
    std::weak_ptr<QAction> action, const HierarchicalId& uniqueId,
    std::string description,
    const std::vector<QKeySequence>& defaultKeySequences)
{
    if (m_actions.find(uniqueId) != m_actions.end()) {
        AIDE_LOG_WARN(
            "Action with id \"{}\" is already registered. This is either a "
            "programming error or results from conflicting plugins. This "
            "action cannot be found with \"Find Action\"  and will not be "
            "displayed in the keymap.",
            uniqueId.name())
        return;
    }

    AIDE_LOG_TRACE(
        R"(Register new action "{}" with description "{}" and default sequence "{}")",
        uniqueId.name(), description, printKeySequences(defaultKeySequences))

    QList<QKeySequence> qtDefaultKeySequences;
    for (const auto& seq : defaultKeySequences) {
        qtDefaultKeySequences << seq;
    }

    Action detailedAction{action, description, qtDefaultKeySequences};
    if (!action.expired()) {
        auto sharedAction = action.lock();
        sharedAction->setStatusTip(QString::fromStdString(description));
        sharedAction->setShortcuts(qtDefaultKeySequences);
    }
    m_actions.insert({uniqueId, detailedAction});
}

std::map<HierarchicalId, Action> ActionRegistry::actions() const
{
    return m_actions;
}

std::string ActionRegistry::printKeySequences(
    const std::vector<QKeySequence>& keySequences)
{
    if (keySequences.empty()) { return ""; }

    std::string combined;

    for (const auto& seq : keySequences) {
        combined += seq.toString().toStdString();
        combined += ", ";
    }
    return combined.erase(combined.size() - 2);
}
