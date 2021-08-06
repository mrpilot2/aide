
#ifndef AIDE_ACTIONREGISTRYINTERFACE_HPP
#define AIDE_ACTIONREGISTRYINTERFACE_HPP

#include <map>
#include <memory>
#include <string>
#include <vector>

#include <QKeySequence>
#include <QList>

#include <aide/hierarchicalid.hpp>

class QAction;
class QKeySequence;

namespace aide
{
    class HierarchicalId;

    struct Action
    {
        std::weak_ptr<QAction> action;
        std::string description;
        QList<QKeySequence> defaultKeySequences;
        QList<QKeySequence> keySequences;

        QList<QKeySequence> getActiveKeySequences() const
        {
            return keySequences.isEmpty() ? defaultKeySequences : keySequences;
        }

        bool areKeySequencesTheSame(QList<QKeySequence> lhs,
                                    QList<QKeySequence> rhs) const
        {
            if ((lhs.size() == 1 && lhs.at(0).isEmpty() && rhs.size() == 0) ||
                (rhs.size() == 1 && rhs.at(0).isEmpty() && lhs.size() == 0)) {
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

        virtual const std::map<HierarchicalId, Action>& actions() const = 0;
    };

    using ActionRegistryInterfacePtr = std::shared_ptr<ActionRegistryInterface>;
} // namespace aide

#endif // AIDE_ACTIONREGISTRYINTERFACE_HPP
