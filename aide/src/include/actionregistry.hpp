#ifndef AIDE_ACTIONREGISTRY_HPP
#define AIDE_ACTIONREGISTRY_HPP

#include <QAction>
#include <QKeySequence>

#include <aide/actionregistryinterface.hpp>

#include "hierarchicalid.hpp"

namespace aide
{
    class ActionRegistry : public ActionRegistryInterface
    {
    public:
        void registerAction(std::weak_ptr<QAction> action,
                            const HierarchicalId& uniqueId) override;

        void registerAction(std::weak_ptr<QAction> action,
                            const HierarchicalId& uniqueId,
                            std::string description) override;

        void registerAction(
            std::weak_ptr<QAction> action, const HierarchicalId& uniqueId,
            const std::vector<QKeySequence>& defaultKeySequences) override;

        void registerAction(
            std::weak_ptr<QAction> action, const HierarchicalId& uniqueId,
            std::string description,
            const std::vector<QKeySequence>& defaultKeySequences) override;

        [[nodiscard]] std::map<HierarchicalId, Action> actions() const override;

    private:
        static std::string printKeySequences(
            const std::vector<QKeySequence>& keySequences);

        std::map<HierarchicalId, Action> m_actions;
    };
} // namespace aide

#endif // AIDE_ACTIONREGISTRY_HPP
