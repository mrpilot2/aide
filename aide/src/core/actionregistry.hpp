#ifndef AIDE_ACTION_REGISTRY_HPP
#define AIDE_ACTION_REGISTRY_HPP

#include <optional>
#include <settingsinterface.hpp>

#include <QAction>
#include <QKeySequence>

#include <aide/actionregistryinterface.hpp>
#include <aide/hierarchicalid.hpp>
#include <aide/loggerinterface.hpp>
#include <aide/menucontainerinterface.hpp>

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

        MenuContainerInterfacePtr createMenu(
            const HierarchicalId& uniqueId) override;

        MenuContainerInterfacePtr createMenu(
            const HierarchicalId& uniqueId, QWidget* parent) override;

        [[nodiscard]] std::optional<MenuContainerInterfacePtr> getMenuContainer(
            const HierarchicalId& uniqueId) const override;

    private:
        static std::string printKeySequences(
            const std::vector<QKeySequence>& keySequences);

        QList<QKeySequence> loadUserKeySequences(
            const HierarchicalId& uniqueId);

        SettingsInterface& settings;

        LoggerPtr logger;
        std::map<HierarchicalId, Action> m_actions;

        std::map<HierarchicalId, MenuContainerInterfacePtr> m_menus;
    };
} // namespace aide

#endif // AIDE_ACTION_REGISTRY_HPP
