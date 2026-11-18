#include <memory>

#include <catch2/catch_test_macros.hpp>

#include <QDialogButtonBox>
#include <QKeySequence>
#include <QLineEdit>
#include <QPushButton>
#include <QStandardItemModel>
#include <QTest>
#include <QTreeView>
#include <QWidget>

#include <aide/helptopicid.hpp>
#include <aide/widgethelptopic.hpp>

#include "settings/settingsdialog.hpp"
#include "settings/settingsdialogcontroller.hpp"
#include "spychangepagecontroller.hpp"

using aide::HelpTopicId;
using aide::gui::SettingsDialog;
using aide::gui::SettingsDialogController;
using aide::test::SpyChangePageController;

namespace
{
    QPushButton* helpButton(const SettingsDialog& dialog)
    {
        const auto* box = dialog.findChild<QDialogButtonBox*>("helpButtonBox");
        return box == nullptr ? nullptr : box->button(QDialogButtonBox::Help);
    }

    // Shows and activates @p dialog with a controller forwarding to @p spy,
    // and puts the focus into its page tree.
    QTreeView* focusPageTree(SettingsDialog& dialog,
                             SpyChangePageController& spy)
    {
        dialog.setController(std::make_shared<SettingsDialogController>(spy));
        dialog.show();
        dialog.activateWindow();
        REQUIRE(QTest::qWaitForWindowActive(&dialog));

        auto* tree = dialog.findChild<QTreeView*>("treeView");
        REQUIRE(tree != nullptr);
        tree->setFocus();
        return tree;
    }
} // namespace

TEST_CASE("SettingsDialog programmatic page selection", "[SettingsDialog]")
{
    // SettingsDialog wires its tree selection to the controller, so a dialog
    // without one makes connect() fail with a Qt warning.
    SpyChangePageController changePageController;
    SettingsDialog dialog;
    dialog.setController(std::make_shared<aide::gui::SettingsDialogController>(
        changePageController));

    auto model = std::make_shared<QStandardItemModel>();
    model->appendRow(new QStandardItem("Alpha"));
    model->appendRow(new QStandardItem("Beta"));
    model->appendRow(new QStandardItem("Gamma"));
    dialog.setTreeModel(model);

    auto* tree = dialog.findChild<QTreeView*>("treeView");
    REQUIRE(tree != nullptr);

    SECTION("selecting successive pages keeps exactly one row selected")
    {
        dialog.setSelectedGroupIndex(model->index(0, 0));
        dialog.setSelectedGroupIndex(model->index(2, 0));

        REQUIRE(tree->selectionModel()->selectedRows().size() == 1);
        REQUIRE(tree->selectionModel()->selectedRows().at(0).row() == 2);
    }

    SECTION("re-selecting the same page still leaves a single row selected")
    {
        dialog.setSelectedGroupIndex(model->index(1, 0));
        dialog.setSelectedGroupIndex(model->index(1, 0));

        REQUIRE(tree->selectionModel()->selectedRows().size() == 1);
        REQUIRE(tree->selectionModel()->selectedRows().at(0).row() == 1);
    }
}

TEST_CASE("SettingsDialog shows a hidden page widget", "[SettingsDialog]")
{
    SettingsDialog dialog;

    // Pages are hidden up front when the dialog opens; showing a page must make
    // its widget visible again instead of leaving an empty panel. The scroll
    // area takes ownership of the widget, so it is freed with the dialog.
    auto* page = new QWidget;
    page->hide();
    REQUIRE(page->isHidden());

    dialog.showSelectedPageWidget(page);

    REQUIRE_FALSE(page->isHidden());
}

TEST_CASE("SettingsDialog does not free a page it does not own",
          "[SettingsDialog]")
{
    // Settings pages are owned by the SettingsPageRegistry via shared_ptr,
    // typically a make_shared allocation where the widget lives inside the
    // control block. Showing a page hands it to the scroll area, which takes
    // Qt ownership. When the dialog is destroyed it must release the current
    // page instead of letting Qt delete/free a widget it does not own -
    // otherwise the make_shared block is freed twice (heap corruption / abort).
    auto page = std::make_shared<QWidget>();

    {
        SettingsDialog dialog;
        dialog.showSelectedPageWidget(page.get());
    }

    // The shared_ptr must still be the sole, valid owner after the dialog dies.
    REQUIRE(page.use_count() == 1);
    REQUIRE(page->parent() == nullptr);
}

TEST_CASE("SettingsDialog Help button", "[SettingsDialog][Help]")
{
    SettingsDialog dialog;
    auto* button = helpButton(dialog);
    REQUIRE(button != nullptr);

    SECTION("is disabled initially")
    {
        REQUIRE_FALSE(button->isEnabled());
    }

    SECTION("can be hidden and shown")
    {
        dialog.showHelpButton(false);
        REQUIRE(button->isHidden());

        dialog.showHelpButton(true);
        REQUIRE_FALSE(button->isHidden());
    }

    SECTION("can be enabled and disabled")
    {
        dialog.enableHelpButton(true);
        REQUIRE(button->isEnabled());

        dialog.enableHelpButton(false);
        REQUIRE_FALSE(button->isEnabled());
    }

    SECTION("shows help for the current page when clicked")
    {
        SpyChangePageController spy;
        dialog.setController(std::make_shared<SettingsDialogController>(spy));
        dialog.enableHelpButton(true);

        button->click();

        REQUIRE(spy.showHelpForCurrentPageCalled);
    }
}

TEST_CASE("SettingsDialog binds a Help Topic to itself",
          "[SettingsDialog][Help]")
{
    SettingsDialog dialog;

    dialog.setHelpTopic(HelpTopicId{"keymap"});
    REQUIRE(aide::helpTopic(&dialog) == HelpTopicId{"keymap"});

    dialog.setHelpTopic(HelpTopicId{});
    REQUIRE(aide::helpTopic(&dialog).isEmpty());
}

TEST_CASE("SettingsDialog Context Help shortcut", "[SettingsDialog][Help]")
{
    const QKeySequence defaultKey(QKeySequence::HelpContents);
    SettingsDialog dialog;
    SpyChangePageController spy;
    auto* tree = focusPageTree(dialog, spy);
    dialog.setHelpTopic(HelpTopicId{"keymap"});

    SECTION("resolves focus in the page tree to the dialog's topic")
    {
        dialog.setContextHelpShortcuts({defaultKey});

        QTest::keySequence(tree, defaultKey);

        REQUIRE(spy.showContextHelpCalled);
        REQUIRE(spy.lastContextHelpTopic == HelpTopicId{"keymap"});
    }

    SECTION("prefers the topic of a focused widget inside the page")
    {
        auto* lineEdit = new QLineEdit;
        aide::setHelpTopic(lineEdit, HelpTopicId{"keymap/search"});
        dialog.showSelectedPageWidget(lineEdit);
        dialog.setContextHelpShortcuts({defaultKey});
        lineEdit->setFocus();

        QTest::keySequence(lineEdit, defaultKey);

        REQUIRE(spy.lastContextHelpTopic == HelpTopicId{"keymap/search"});
    }

    SECTION("does not exist without keys")
    {
        dialog.setContextHelpShortcuts({});

        QTest::keySequence(tree, defaultKey);

        REQUIRE_FALSE(spy.showContextHelpCalled);
    }

    SECTION("uses only the latest keys")
    {
        const QKeySequence userKey(Qt::CTRL | Qt::SHIFT | Qt::Key_H);
        dialog.setContextHelpShortcuts({defaultKey});
        dialog.setContextHelpShortcuts({userKey});

        QTest::keySequence(tree, defaultKey);
        REQUIRE_FALSE(spy.showContextHelpCalled);

        QTest::keySequence(tree, userKey);
        REQUIRE(spy.showContextHelpCalled);
    }
}
