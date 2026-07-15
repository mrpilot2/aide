#include <memory>

#include <catch2/catch_test_macros.hpp>

#include <QAction>
#include <QKeySequence>

#include "actionregistry.hpp"
#include "hierarchicalid.hpp"
#include "mockkeymappagewidget.hpp"
#include "mocksettings.hpp"
#include "nulllogger.hpp"
#include "settings/keymap/keymappagewidgetcontroller.hpp"
#include "settings/keymap/keymaptreemodel.hpp"

using aide::ActionRegistry;
using aide::HierarchicalId;
using aide::core::KeyMapTreeModel;
using aide::gui::KeyMapPageWidgetController;
using aide::test::MockKeyMapPageWidget;
using aide::test::MockSettings;
using aide::test::NullLogger;

TEST_CASE("A key map page widget controller handling an existing shortcut")
{
    MockSettings settings;
    auto logger = std::make_shared<NullLogger>();
    auto registry(std::make_shared<ActionRegistry>(settings, logger));

    auto action{std::make_shared<QAction>()};
    const QKeySequence defaultSequence(Qt::CTRL | Qt::Key_N);
    registry->registerAction(action, HierarchicalId("New File"), "",
                             {defaultSequence});

    auto treeModel = std::make_shared<KeyMapTreeModel>(registry);
    MockKeyMapPageWidget view;
    KeyMapPageWidgetController controller(treeModel, &view);

    const QModelIndex actionIndex = treeModel->index(0, 0);
    controller.requestContextMenuForIndex(actionIndex);
    const QModelIndex shortcutsIndex = treeModel->index(0, 1);

    SECTION("resets the current shortcuts to the action's default")
    {
        const QKeySequence customSequence(Qt::Key_X);
        treeModel->setData(shortcutsIndex,
                           QKeySequence::listToString({customSequence}),
                           Qt::DisplayRole);

        controller.onUserRequestedToResetCurrentShortcutsToDefault();

        REQUIRE(treeModel->data(shortcutsIndex, Qt::DisplayRole).toString() ==
                QKeySequence::listToString({defaultSequence}));
    }

    SECTION("removes only the shortcut carried by the triggering action")
    {
        const QKeySequence otherSequence(Qt::Key_X);
        treeModel->setData(
            shortcutsIndex,
            QKeySequence::listToString({defaultSequence, otherSequence}),
            Qt::DisplayRole);

        QAction removeAction;
        removeAction.setData(otherSequence);
        // String-based connect on purpose: a pointer-to-member connect
        // references the sender's and receiver's staticMetaObject data
        // symbols, which are not exported across DLL boundaries on MSVC
        // (CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS exports functions, not data).
        // The string form resolves the signal/slot at runtime via the
        // exported virtual metaObject(), so it links there.
        QObject::connect(&removeAction, SIGNAL(triggered()), &controller,
                         SLOT(onUserRequestedToRemoveAShortcut()));

        removeAction.trigger();

        REQUIRE(treeModel->data(shortcutsIndex, Qt::DisplayRole).toString() ==
                QKeySequence::listToString({defaultSequence}));
    }
}
