#include <memory>

#include <catch2/catch_test_macros.hpp>

#include <QStandardItemModel>
#include <QTreeView>

#include "settings/settingsdialog.hpp"

using aide::gui::SettingsDialog;

TEST_CASE("SettingsDialog programmatic page selection", "[SettingsDialog]")
{
    SettingsDialog dialog;

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
