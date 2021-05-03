#ifndef AIDE_ACTIONREGISTRY_HPP
#define AIDE_ACTIONREGISTRY_HPP

#include <QAction>
#include <QKeySequence>

#include <aide/actionregistryinterface.hpp>
#include <aide/hierarchicalid.hpp>
#include <aide/loggerinterface.hpp>

namespace aide
{
    class ActionRegistry : public ActionRegistryInterface
    {
    public:
        explicit ActionRegistry(LoggerPtr loggerInterface);

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

        [[nodiscard]] const std::map<HierarchicalId, Action>& actions() const override;

    private:
        static std::string printKeySequences(
            const std::vector<QKeySequence>& keySequences);

        LoggerPtr logger;

        std::map<HierarchicalId, Action> m_actions;
    };
} // namespace aide

#endif // AIDE_ACTIONREGISTRY_HPP
