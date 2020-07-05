
#ifndef AIDE_ACTIONREGISTRYINTERFACE_HPP
#define AIDE_ACTIONREGISTRYINTERFACE_HPP

#include <memory>
#include <string>

class QAction;
class QKeySequence;

namespace aide
{
    class HierarchicalId;

    struct Action
    {
        std::weak_ptr<QAction> action;
        std::string description;
        QKeySequence defaultKeySequence;
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

        virtual void registerAction(std::weak_ptr<QAction> action,
                                    const HierarchicalId& uniqueId,
                                    QKeySequence defaultKeySequence) = 0;

        virtual void registerAction(std::weak_ptr<QAction> action,
                                    const HierarchicalId& uniqueId,
                                    std::string description,
                                    QKeySequence defaultKeySequence) = 0;

        virtual std::map<HierarchicalId, Action> actions() const = 0;
    };
} // namespace aide

#endif // AIDE_ACTIONREGISTRYINTERFACE_HPP
