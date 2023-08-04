#include "actionregistry.hpp"

#include "menucontainer.hpp"

using aide::Action;
using aide::ActionRegistry;
using aide::HierarchicalId;
using aide::LoggerPtr;
using aide::MenuContainerInterfacePtr;

ActionRegistry::ActionRegistry(SettingsInterface& settingsInterface,
                               LoggerPtr loggerInterface)
    : settings{settingsInterface}
    , logger{std::move(loggerInterface)}
{}

void ActionRegistry::registerAction(std::weak_ptr<QAction> action,
                                    const HierarchicalId& uniqueId)
{
    registerAction(action, uniqueId, "", {});
}

void ActionRegistry::registerAction(std::weak_ptr<QAction> action,
                                    const HierarchicalId& uniqueId,
                                    std::string description)
{
    registerAction(action, uniqueId, std::move(description), {});
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
        logger->warn(
            R"(Action with id "{}" is already registered. This is either a "
            "programming error or results from conflicting plugins. This "
            "action cannot be found with "Find Action"  and will not be "
            "displayed in the keymap.")",
            uniqueId.name());
        return;
    }

    logger->trace(
        R"(Register new action "{}" with description "{}" and default sequence "{}")",
        uniqueId.name(), description, printKeySequences(defaultKeySequences));

    QList<QKeySequence> qtDefaultSequences;
    for (const auto& seq : defaultKeySequences) {
        qtDefaultSequences << seq;
    }

    auto userKeySequences = loadUserKeySequences(uniqueId);

    Action const detailedAction{action, description, qtDefaultSequences,
                                userKeySequences};

    if (!action.expired()) {
        auto sharedAction = action.lock();
        sharedAction->setStatusTip(QString::fromStdString(description));
        auto shortcuts = detailedAction.getActiveKeySequences();
        sharedAction->setShortcuts(
            (shortcuts.size() == 1 && shortcuts.at(0).isEmpty())
                ? QList<QKeySequence>()
                : shortcuts);
    }
    m_actions.try_emplace(uniqueId, detailedAction);
}

QList<QKeySequence> ActionRegistry::loadUserKeySequences(
    const HierarchicalId& uniqueId)
{
    auto settingsId{HierarchicalId("Keymap")};
    for (const auto* i : uniqueId) {
        settingsId.addLevel(i);
    }
    if (settings.value(settingsId) == QVariant()) { return {}; }
    return QKeySequence::listFromString(settings.value(settingsId).toString());
}

void aide::ActionRegistry::modifyShortcutsForAction(
    HierarchicalId id, const QList<QKeySequence>& shortcuts)
{
    auto settingsId{HierarchicalId("Keymap")};
    for (const auto* i : id) {
        settingsId.addLevel(i);
    }

    auto& action = m_actions.at(id);

    action.action.lock()->setShortcuts(shortcuts);

    if (Action::areKeySequencesTheSame(shortcuts, action.defaultKeySequences)) {
        action.keySequences.clear();
        settings.removeKey(settingsId);
    } else {
        action.keySequences = shortcuts;
        settings.setValue(settingsId, QKeySequence::listToString(shortcuts));
    }
}

const std::map<HierarchicalId, Action>& ActionRegistry::actions() const
{
    return m_actions;
}

std::optional<QAction*> ActionRegistry::action(const HierarchicalId& id) const
{
    if (auto it = m_actions.find(id); it != m_actions.end()) {
        return it->second.action.lock().get();
    }

    logger->warn(
        "Could not find action by ID {}. Returning empty optional instead.",
        id.name());
    return {};
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

MenuContainerInterfacePtr ActionRegistry::createMenu(
    const aide::HierarchicalId& uniqueId)
{
    return createMenu(uniqueId, nullptr);
}

MenuContainerInterfacePtr ActionRegistry::createMenu(
    const HierarchicalId& uniqueId, QWidget* parent)
{
    if (m_menus.find(uniqueId) == m_menus.end()) {
        m_menus.try_emplace(uniqueId, std::make_shared<MenuContainer>(parent));
    }

    return m_menus.at(uniqueId);
}

std::optional<MenuContainerInterfacePtr> aide::ActionRegistry::getMenuContainer(
    const HierarchicalId& uniqueId) const
{
    if (auto it = m_menus.find(uniqueId); it != m_menus.end()) {
        return it->second;
    }

    logger->warn(
        "Could not find MenuContainer for Id {}. Returning empty optional "
        "instead.",
        uniqueId.name());
    return {};
}
