#include "actionregistry.hpp"

#include <aide/logger.hpp>

using aide::Action;
using aide::ActionRegistry;
using aide::HierarchicalId;

void ActionRegistry::registerAction(std::weak_ptr<QAction> action,
                                    const HierarchicalId& uniqueId)
{
    registerAction(action, uniqueId, "", QKeySequence());
}

void ActionRegistry::registerAction(std::weak_ptr<QAction> action,
                                    const HierarchicalId& uniqueId,
                                    std::string description)
{
    registerAction(action, uniqueId, description, QKeySequence());
}

void ActionRegistry::registerAction(std::weak_ptr<QAction> action,
                                    const HierarchicalId& uniqueId,
                                    QKeySequence defaultKeySequence)
{
    registerAction(action, uniqueId, "", defaultKeySequence);
}

void ActionRegistry::registerAction(std::weak_ptr<QAction> action,
                                    const HierarchicalId& uniqueId,
                                    std::string description,
                                    QKeySequence defaultKeySequence)
{
    AIDE_LOG_TRACE(
        R"(Register new action "{}" with description "{}" and default sequence "{}")",
        uniqueId.name(), description,
        defaultKeySequence.toString().toStdString())

    Action detailedAction{action, description, defaultKeySequence};
    if (!action.expired()) {
        auto sharedAction = action.lock();
        sharedAction->setStatusTip(QString::fromStdString(description));
        sharedAction->setShortcut(defaultKeySequence);
    }
    m_actions.insert({uniqueId, detailedAction});
}

std::map<HierarchicalId, Action> ActionRegistry::actions() const
{
    return m_actions;
}
