#include <array>
#include <map>
#include <optional>

#include <catch2/catch_test_macros.hpp>

#include <QKeySequence>
#include <QList>

#include <aide/actionregistryinterface.hpp>
#include <aide/hierarchicalid.hpp>

#include "settings/keymap/pendingkeymapstate.hpp"

using aide::Action;
using aide::ActionRegistryInterface;
using aide::ActionRegistryInterfacePtr;
using aide::HierarchicalId;
using aide::MenuContainerInterface;
using aide::MenuContainerInterfacePtr;
using aide::core::PendingKeymapState;

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

TEST_CASE("A new PendingKeymapState with no actions")
{
    auto registry = std::make_shared<StubRegistry>();
    const PendingKeymapState state(registry);

    SECTION("is not dirty")
    {
        REQUIRE_FALSE(state.isAnyDirty());
    }
}

TEST_CASE("PendingKeymapState with one registered action")
{
    const auto saveId = HierarchicalId("file")("save");

    auto registry = std::make_shared<StubRegistry>();
    registry->addAction(saveId, {QKeySequence("Ctrl+S")});

    PendingKeymapState state(registry);

    SECTION("is not dirty initially")
    {
        REQUIRE_FALSE(state.isDirty(saveId));
        REQUIRE_FALSE(state.isAnyDirty());
    }

    SECTION("returns baseline shortcut as pending before any edit")
    {
        REQUIRE(state.pendingShortcuts(saveId) ==
                QList<QKeySequence>{QKeySequence("Ctrl+S")});
    }

    SECTION("becomes dirty after setPendingShortcuts")
    {
        state.setPendingShortcuts(saveId, {QKeySequence("Ctrl+W")});

        REQUIRE(state.isDirty(saveId));
        REQUIRE(state.isAnyDirty());
    }

    SECTION("pending shortcut reflects the new value after edit")
    {
        state.setPendingShortcuts(saveId, {QKeySequence("Ctrl+W")});

        REQUIRE(state.pendingShortcuts(saveId) ==
                QList<QKeySequence>{QKeySequence("Ctrl+W")});
    }

    SECTION("revertAction makes it clean again")
    {
        state.setPendingShortcuts(saveId, {QKeySequence("Ctrl+W")});
        state.revertAction(saveId);

        REQUIRE_FALSE(state.isDirty(saveId));
        REQUIRE(state.pendingShortcuts(saveId) ==
                QList<QKeySequence>{QKeySequence("Ctrl+S")});
    }

    SECTION("reset makes it clean again after edit")
    {
        state.setPendingShortcuts(saveId, {QKeySequence("Ctrl+W")});
        state.reset();

        REQUIRE_FALSE(state.isAnyDirty());
    }

    SECTION("commitToRegistry writes dirty entries and clears dirty flag")
    {
        state.setPendingShortcuts(saveId, {QKeySequence("Ctrl+W")});
        REQUIRE(state.isAnyDirty());

        state.commitToRegistry();

        REQUIRE_FALSE(state.isAnyDirty());
        REQUIRE(registry->actions().at(saveId).keySequences ==
                QList<QKeySequence>{QKeySequence("Ctrl+W")});
    }

    SECTION("commitToRegistry does not write clean entries")
    {
        state.commitToRegistry();

        REQUIRE(registry->actions().at(saveId).keySequences.isEmpty());
    }

    SECTION("pendingShortcuts returns empty list for an unregistered id")
    {
        const auto unknownId = HierarchicalId("unknown")("action");

        REQUIRE(state.pendingShortcuts(unknownId).isEmpty());
    }
}
