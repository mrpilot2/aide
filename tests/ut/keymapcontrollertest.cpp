#include <array>
#include <map>
#include <optional>

#include <catch2/catch_test_macros.hpp>

#include <QKeySequence>
#include <QList>

#include <aide/actionregistryinterface.hpp>
#include <aide/hierarchicalid.hpp>

#include "settings/keymap/keymapcontroller.hpp"

using aide::Action;
using aide::ActionRegistryInterface;
using aide::ActionRegistryInterfacePtr;
using aide::HierarchicalId;
using aide::MenuContainerInterface;
using aide::MenuContainerInterfacePtr;
using aide::core::KeymapController;

namespace
{
    class StubRegistry : public ActionRegistryInterface
    {
    public:
        void addAction(const HierarchicalId& id,
                       const QList<QKeySequence>& defaultSequences = {})
        {
            Action act;
            act.defaultKeySequences = defaultSequences;
            m_actions[id]           = act;
        }

        [[nodiscard]] QList<QKeySequence> currentShortcuts(
            const HierarchicalId& id) const
        {
            if (!m_actions.contains(id)) { return {}; }
            return m_actions.at(id).keySequences;
        }

        void modifyShortcutsForAction(
            HierarchicalId id, const QList<QKeySequence>& shortcuts) override
        {
            m_actions[id].keySequences = shortcuts;
        }

        [[nodiscard]] const std::map<HierarchicalId, Action>& actions()
            const override
        {
            return m_actions;
        }

        void registerAction(std::weak_ptr<QAction> /*action*/,
                            const HierarchicalId& /*uniqueId*/) override
        {}
        void registerAction(std::weak_ptr<QAction> /*action*/,
                            const HierarchicalId& /*uniqueId*/,
                            std::string /*description*/) override
        {}
        void registerAction(
            std::weak_ptr<QAction> /*action*/,
            const HierarchicalId& /*uniqueId*/,
            const std::vector<QKeySequence>& /*defaultKeySequences*/) override
        {}
        void registerAction(
            std::weak_ptr<QAction> /*action*/,
            const HierarchicalId& /*uniqueId*/, std::string /*description*/,
            const std::vector<QKeySequence>& /*defaultKeySequences*/) override
        {}
        [[nodiscard]] std::optional<QAction*> action(
            const HierarchicalId& /*id*/) const override
        {
            return {};
        }
        MenuContainerInterface* createMenu(
            const HierarchicalId& /*uniqueId*/) override
        {
            return nullptr;
        }
        MenuContainerInterface* createMenu(const HierarchicalId& /*uniqueId*/,
                                           QWidget* /*parent*/) override
        {
            return nullptr;
        }
        [[nodiscard]] auto getMenuContainer(const HierarchicalId& /*uniqueId*/)
            const -> std::optional<MenuContainerInterface*> override
        {
            return {};
        }

        [[nodiscard]] const std::map<HierarchicalId, MenuContainerInterfacePtr>&
        menus() const override
        {
            return m_menus;
        }

    private:
        std::map<HierarchicalId, Action> m_actions;
        std::map<HierarchicalId, MenuContainerInterfacePtr> m_menus;
    };
} // namespace

TEST_CASE("A new KeymapController with no actions registered")
{
    auto registry = std::make_shared<StubRegistry>();
    const KeymapController controller(registry);

    SECTION("is not modified")
    {
        REQUIRE_FALSE(controller.isModified());
    }

    SECTION("exposes a non-null tree model")
    {
        REQUIRE(controller.treeModel() != nullptr);
    }
}

TEST_CASE("KeymapController with one registered action")
{
    const auto saveId = HierarchicalId("file")("save");

    auto registry = std::make_shared<StubRegistry>();
    registry->addAction(saveId, {QKeySequence("Ctrl+S")});

    KeymapController controller(registry);

    SECTION("is not modified before any change")
    {
        REQUIRE_FALSE(controller.isModified());
    }

    SECTION("becomes modified after setPendingShortcuts")
    {
        controller.setPendingShortcuts(saveId, {QKeySequence("Ctrl+W")});

        REQUIRE(controller.isModified());
    }

    SECTION("pendingShortcuts returns what was set")
    {
        controller.setPendingShortcuts(saveId, {QKeySequence("Ctrl+W")});

        REQUIRE(controller.pendingShortcuts(saveId) ==
                QList<QKeySequence>{QKeySequence("Ctrl+W")});
    }

    SECTION("revertAction removes the pending change and restores clean state")
    {
        controller.setPendingShortcuts(saveId, {QKeySequence("Ctrl+W")});
        REQUIRE(controller.isModified());

        controller.revertAction(saveId);

        REQUIRE_FALSE(controller.isModified());
    }

    SECTION("apply commits the pending shortcut to the registry")
    {
        controller.setPendingShortcuts(saveId, {QKeySequence("Ctrl+W")});
        controller.apply();

        REQUIRE(registry->currentShortcuts(saveId) ==
                QList<QKeySequence>{QKeySequence("Ctrl+W")});
    }

    SECTION("apply clears the modified state")
    {
        controller.setPendingShortcuts(saveId, {QKeySequence("Ctrl+W")});
        controller.apply();

        REQUIRE_FALSE(controller.isModified());
    }

    SECTION("reset discards pending change and clears modified state")
    {
        controller.setPendingShortcuts(saveId, {QKeySequence("Ctrl+W")});
        controller.reset();

        REQUIRE_FALSE(controller.isModified());
    }

    SECTION("reset does not commit the pending shortcut to the registry")
    {
        controller.setPendingShortcuts(saveId, {QKeySequence("Ctrl+W")});
        controller.reset();

        REQUIRE(registry->currentShortcuts(saveId).isEmpty());
    }
}
