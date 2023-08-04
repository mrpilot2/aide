
#ifndef AIDE_ACTION_REGISTRY_INTERFACE_HPP
#define AIDE_ACTION_REGISTRY_INTERFACE_HPP

#include <map>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include <QKeySequence>
#include <QList>

#include <aide/menucontainerinterface.hpp>

class QAction;
class QKeySequence;
class QWidget;

namespace aide
{
    class HierarchicalId;

    struct Action
    {
        std::weak_ptr<QAction> action;
        std::string description;
        QList<QKeySequence> defaultKeySequences;
        QList<QKeySequence> keySequences;

        [[nodiscard]] QList<QKeySequence> getActiveKeySequences() const
        {
            return keySequences.isEmpty() ? defaultKeySequences : keySequences;
        }

        static bool areKeySequencesTheSame(const QList<QKeySequence>& lhs,
                                           const QList<QKeySequence>& rhs)
        {
            if ((lhs.size() == 1 && lhs.at(0).isEmpty() && rhs.isEmpty()) ||
                (rhs.size() == 1 && rhs.at(0).isEmpty() && lhs.isEmpty())) {
                return true;
            }

            return lhs == rhs;
        }
    };

    class ActionRegistryInterface
    {
    public:
        virtual ~ActionRegistryInterface() = default;

        virtual void registerAction(std::weak_ptr<QAction> action,
                                    const HierarchicalId& uniqueId) = 0;

        virtual void registerAction(std::weak_ptr<QAction> action,
                                    const HierarchicalId& uniqueId,
                                    std::string description) = 0;

        virtual void registerAction(
            std::weak_ptr<QAction> action, const HierarchicalId& uniqueId,
            const std::vector<QKeySequence>& defaultKeySequences) = 0;

        virtual void registerAction(
            std::weak_ptr<QAction> action, const HierarchicalId& uniqueId,
            std::string description,
            const std::vector<QKeySequence>& defaultKeySequences) = 0;

        virtual void modifyShortcutsForAction(
            HierarchicalId id, const QList<QKeySequence>& shortcuts) = 0;

        [[nodiscard]] virtual const std::map<HierarchicalId, Action>& actions()
            const = 0;

        [[nodiscard]] virtual std::optional<QAction*> action(
            const HierarchicalId& id) const = 0;

        virtual MenuContainerInterfacePtr createMenu(
            const HierarchicalId& uniqueId) = 0;

        virtual MenuContainerInterfacePtr createMenu(
            const HierarchicalId& uniqueId, QWidget* parent) = 0;

        [[nodiscard]] virtual std::optional<MenuContainerInterfacePtr>
        getMenuContainer(const HierarchicalId& uniqueId) const = 0;
    };

    using ActionRegistryInterfacePtr = std::shared_ptr<ActionRegistryInterface>;
} // namespace aide

#endif // AIDE_ACTION_REGISTRY_INTERFACE_HPP
