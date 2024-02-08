#ifndef AIDE_ACTION_REGISTRY_HPP
#define AIDE_ACTION_REGISTRY_HPP

#include <optional>
#include <settingsinterface.hpp>

#include <QAction>
#include <QKeySequence>

#include <aide/actionregistryinterface.hpp>
#include <aide/hierarchicalid.hpp>
#include <aide/loggerinterface.hpp>

namespace aide
{
    class SettingsInterface;

    class ActionRegistry : public ActionRegistryInterface
    {
    public:
        explicit ActionRegistry(SettingsInterface& settingsInterface,
                                LoggerPtr loggerInterface);

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

        void modifyShortcutsForAction(
            HierarchicalId id, const QList<QKeySequence>& shortcuts) override;

        [[nodiscard]] const std::map<HierarchicalId, Action>& actions()
            const override;

        std::optional<QAction*> action(const HierarchicalId& id) const override;

        MenuContainerInterface* createMenu(
            const aide::HierarchicalId& uniqueId) override;

        MenuContainerInterface* createMenu(const HierarchicalId& uniqueId,
                                           QWidget* parent) override;

        [[nodiscard]] std::optional<MenuContainerInterface*> getMenuContainer(
            const HierarchicalId& uniqueId) const override;

    private:
        static std::string printKeySequences(
            const std::vector<QKeySequence>& keySequences);

        QList<QKeySequence> loadUserKeySequences(
            const HierarchicalId& uniqueId) const;

        SettingsInterface& settings;

        LoggerPtr logger;
        std::map<HierarchicalId, Action> m_actions;

        std::map<HierarchicalId, MenuContainerInterfacePtr> m_menus;
    };
} // namespace aide

#endif // AIDE_ACTION_REGISTRY_HPP
