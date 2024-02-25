#include <stdexcept>

#include <catch2/catch_test_macros.hpp>

#include <QItemSelection>

#include "commonsettingskeys.hpp"
#include "hierarchicalid.hpp"
#include "mocksettings.hpp"
#include "mocksettingsdialog.hpp"
#include "mocksettingspage.hpp"
#include "nulllogger.hpp"
#include "settings/settingspageregistry.hpp"
#include "settings/showsettingsdialog.hpp"
#include "settings/showsettingsdialogcontroller.hpp"

using aide::HierarchicalId;
using aide::core::SettingsPageGroupTreeModel;
using aide::core::SettingsPageRegistry;
using aide::core::ShowSettingsDialog;
using aide::core::ShowSettingsDialogController;
using aide::core::settings::KEYS;
using aide::test::MockSettings;
using aide::test::MockSettingsDialog;
using aide::test::MockSettingsPage;
using aide::test::NullLogger;

TEST_CASE("Any show settings dialog use case")
{
    auto view = std::make_shared<MockSettingsDialog>();
    MockSettings settings;
    auto logger = std::make_shared<NullLogger>();
    ShowSettingsDialog useCase{view, settings, logger};

    SECTION("shows settings dialog if requested by user")
    {
        useCase.showSettingsDialog();

        REQUIRE(view->wasSettingsDialogExecuted());
    }

    SECTION("works with base class pointer")
    {
        std::unique_ptr<ShowSettingsDialogController> base =
            std::make_unique<ShowSettingsDialog>(view, settings, logger);

        base->showSettingsDialog();

        REQUIRE(view->wasSettingsDialogExecuted());
    }

    SECTION("show display name of selected group")
    {
        SettingsPageRegistry::deleteAllPages();

        SettingsPageRegistry::addPage(std::make_unique<MockSettingsPage>(
            HierarchicalId("MockTestPage")("Subpage1")));

        SettingsPageGroupTreeModel treeModel;

        useCase.showSettingsDialog();

        useCase.changeSelectedPage(
            QItemSelection(treeModel.index(0, 0, QModelIndex()),
                           treeModel.index(0, 0, QModelIndex())),
            QItemSelection(treeModel.index(-1, -1, QModelIndex()),
                           treeModel.index(-1, -1, QModelIndex())));

        REQUIRE(view->displayName() == "MockTestPage");
    }

    SECTION("throws an exception if selected group is invalid")
    {
        SettingsPageRegistry::deleteAllPages();

        SettingsPageRegistry::addPage(std::make_unique<MockSettingsPage>(
            HierarchicalId("MockTestPage")("Subpage1")));

        SettingsPageGroupTreeModel treeModel;

        useCase.showSettingsDialog();

        REQUIRE_THROWS_AS(
            useCase.changeSelectedPage(
                QItemSelection(treeModel.index(0, 0, QModelIndex()),
                               treeModel.index(-1, -1, QModelIndex())),
                QItemSelection(treeModel.index(-1, -1, QModelIndex()),
                               treeModel.index(-1, -1, QModelIndex()))),
            std::invalid_argument);
    }

    SECTION("can be used with base class interface to change page")
    {
        auto base =
            std::shared_ptr<aide::core::SettingsDialogChangePageController>(
                std::make_shared<ShowSettingsDialog>(view, settings, logger));

        SettingsPageGroupTreeModel treeModel;

        REQUIRE_THROWS_AS(
            base->changeSelectedPage(
                QItemSelection(treeModel.index(0, 0, QModelIndex()),
                               treeModel.index(0, 0, QModelIndex())),
                QItemSelection(treeModel.index(-1, -1, QModelIndex()),
                               treeModel.index(-1, -1, QModelIndex()))),
            std::logic_error);
    }

    SECTION("show selected page widget")
    {
        SettingsPageRegistry::deleteAllPages();

        SettingsPageRegistry::addPage(
            std::make_unique<MockSettingsPage>(HierarchicalId("MockTestPage")));

        SettingsPageGroupTreeModel treeModel;

        useCase.showSettingsDialog();

        REQUIRE(view->currentlyShownWidget() == nullptr);

        useCase.changeSelectedPage(
            QItemSelection(treeModel.index(0, 0, QModelIndex()),
                           treeModel.index(0, 0, QModelIndex())),
            QItemSelection(treeModel.index(-1, -1, QModelIndex()),
                           treeModel.index(-1, -1, QModelIndex())));

        REQUIRE(view->currentlyShownWidget() != nullptr);
    }

    SECTION(
        "shows reset label if any GUI element has changed and page is modified")
    {
        SettingsPageRegistry::deleteAllPages();

        auto page =
            std::make_shared<MockSettingsPage>(HierarchicalId("MockTestPage"));
        SettingsPageRegistry::addPage(page);

        SettingsPageGroupTreeModel treeModel;

        useCase.showSettingsDialog();

        useCase.changeSelectedPage(
            QItemSelection(treeModel.index(0, 0, QModelIndex()),
                           treeModel.index(0, 0, QModelIndex())),
            QItemSelection(treeModel.index(-1, -1, QModelIndex()),
                           treeModel.index(-1, -1, QModelIndex())));

        page->simulateModified(true);

        useCase.anyGuiElementHasChanged();

        REQUIRE(view->isResetLabelVisible());
    }

    SECTION(
        "hide reset label if any GUI element has changed and page is not "
        "modified")
    {
        SettingsPageRegistry::deleteAllPages();

        auto page =
            std::make_shared<MockSettingsPage>(HierarchicalId("MockTestPage"));
        SettingsPageRegistry::addPage(page);

        SettingsPageGroupTreeModel treeModel;

        useCase.showSettingsDialog();

        useCase.changeSelectedPage(
            QItemSelection(treeModel.index(0, 0, QModelIndex()),
                           treeModel.index(0, 0, QModelIndex())),
            QItemSelection(treeModel.index(-1, -1, QModelIndex()),
                           treeModel.index(-1, -1, QModelIndex())));

        page->simulateModified(false);

        useCase.anyGuiElementHasChanged();

        REQUIRE(!view->isResetLabelVisible());
    }

    SECTION(
        "enables apply button if any GUI element has changed and page is "
        "modified")
    {
        SettingsPageRegistry::deleteAllPages();

        auto page =
            std::make_shared<MockSettingsPage>(HierarchicalId("MockTestPage"));
        SettingsPageRegistry::addPage(page);

        SettingsPageGroupTreeModel treeModel;

        useCase.showSettingsDialog();

        useCase.changeSelectedPage(
            QItemSelection(treeModel.index(0, 0, QModelIndex()),
                           treeModel.index(0, 0, QModelIndex())),
            QItemSelection(treeModel.index(-1, -1, QModelIndex()),
                           treeModel.index(-1, -1, QModelIndex())));

        page->simulateModified(true);

        useCase.anyGuiElementHasChanged();

        REQUIRE(view->isApplyButtonEnabled());
    }

    SECTION(
        "disables apply button if any GUI element has changed and no page is "
        "modified")
    {
        SettingsPageRegistry::deleteAllPages();

        auto page1 =
            std::make_shared<MockSettingsPage>(HierarchicalId("MockTestPage"));
        auto page2 =
            std::make_shared<MockSettingsPage>(HierarchicalId("MockTestPage2"));

        SettingsPageRegistry::addPage(page1);
        SettingsPageRegistry::addPage(page2);

        SettingsPageGroupTreeModel treeModel;

        useCase.showSettingsDialog();

        useCase.changeSelectedPage(
            QItemSelection(treeModel.index(0, 0, QModelIndex()),
                           treeModel.index(0, 0, QModelIndex())),
            QItemSelection(treeModel.index(-1, -1, QModelIndex()),
                           treeModel.index(-1, -1, QModelIndex())));

        page1->simulateModified(false);
        page2->simulateModified(false);

        useCase.anyGuiElementHasChanged();

        REQUIRE(!view->isApplyButtonEnabled());
    }

    SECTION(
        "does not disable apply button if any GUI element has changed and is "
        "modified but another page is modified")
    {
        SettingsPageRegistry::deleteAllPages();

        auto page1 =
            std::make_shared<MockSettingsPage>(HierarchicalId("MockTestPage"));
        auto page2 =
            std::make_shared<MockSettingsPage>(HierarchicalId("MockTestPage2"));

        SettingsPageRegistry::addPage(page1);
        SettingsPageRegistry::addPage(page2);

        SettingsPageGroupTreeModel treeModel;

        useCase.showSettingsDialog();

        useCase.changeSelectedPage(
            QItemSelection(treeModel.index(0, 0, QModelIndex()),
                           treeModel.index(0, 0, QModelIndex())),
            QItemSelection(treeModel.index(-1, -1, QModelIndex()),
                           treeModel.index(-1, -1, QModelIndex())));

        page1->simulateModified(false);
        page2->simulateModified(true);

        useCase.anyGuiElementHasChanged();

        REQUIRE(view->isApplyButtonEnabled());
    }

    SECTION("applies settings for modified pages if user presses apply button")
    {
        SettingsPageRegistry::deleteAllPages();

        auto page1 =
            std::make_shared<MockSettingsPage>(HierarchicalId("MockTestPage"));
        auto page2 =
            std::make_shared<MockSettingsPage>(HierarchicalId("MockTestPage2"));

        SettingsPageRegistry::addPage(page1);
        SettingsPageRegistry::addPage(page2);

        SettingsPageGroupTreeModel treeModel;

        useCase.showSettingsDialog();

        useCase.changeSelectedPage(
            QItemSelection(treeModel.index(0, 0, QModelIndex()),
                           treeModel.index(0, 0, QModelIndex())),
            QItemSelection(treeModel.index(-1, -1, QModelIndex()),
                           treeModel.index(-1, -1, QModelIndex())));

        page1->simulateModified(false);
        page2->simulateModified(true);

        useCase.applyModifiedSettingsPages();

        REQUIRE(!page1->wasApplyCalled());
        REQUIRE(page2->wasApplyCalled());
    }

    SECTION("applies settings for modified pages if user presses Ok button")
    {
        SettingsPageRegistry::deleteAllPages();

        auto page1 =
            std::make_shared<MockSettingsPage>(HierarchicalId("MockTestPage"));
        auto page2 =
            std::make_shared<MockSettingsPage>(HierarchicalId("MockTestPage2"));

        SettingsPageRegistry::addPage(page1);
        SettingsPageRegistry::addPage(page2);

        SettingsPageGroupTreeModel treeModel;

        page1->simulateModified(false);
        page2->simulateModified(true);

        view->simulateUserAcceptsDialog(true);

        useCase.showSettingsDialog();

        REQUIRE(!page1->wasApplyCalled());
        REQUIRE(page2->wasApplyCalled());
    }

    SECTION("resets currently shown page if user presses reset label")
    {
        SettingsPageRegistry::deleteAllPages();

        auto page1 =
            std::make_shared<MockSettingsPage>(HierarchicalId("MockTestPage"));
        auto page2 =
            std::make_shared<MockSettingsPage>(HierarchicalId("MockTestPage2"));

        SettingsPageRegistry::addPage(page1);
        SettingsPageRegistry::addPage(page2);

        SettingsPageGroupTreeModel treeModel;

        useCase.showSettingsDialog();

        useCase.changeSelectedPage(
            QItemSelection(treeModel.index(1, 0, QModelIndex()),
                           treeModel.index(1, 0, QModelIndex())),
            QItemSelection(treeModel.index(-1, -1, QModelIndex()),
                           treeModel.index(-1, -1, QModelIndex())));

        page1->simulateModified(true);
        page2->simulateModified(true);

        // reset is also called when dialog is shown for the first time
        page1->clearResetWasCalled();
        page2->clearResetWasCalled();

        useCase.resetCurrentPage();

        REQUIRE(!page1->wasResetCalled());
        REQUIRE(page2->wasResetCalled());
    }

    SECTION("resets all modified pages if user cancels")
    {
        SettingsPageRegistry::deleteAllPages();

        auto page1 =
            std::make_shared<MockSettingsPage>(HierarchicalId("MockTestPage"));
        auto page2 =
            std::make_shared<MockSettingsPage>(HierarchicalId("MockTestPage2"));

        SettingsPageRegistry::addPage(page1);
        SettingsPageRegistry::addPage(page2);

        SettingsPageGroupTreeModel treeModel;

        page1->simulateModified(true);
        page2->simulateModified(false);

        view->simulateUserAcceptsDialog(false);

        useCase.showSettingsDialog();

        // once when dialog is shown for the first time and once when dialog
        // is canceled
        REQUIRE(page1->numberOfTimesResetWasCalled() == 2);
        REQUIRE(page2->numberOfTimesResetWasCalled() == 1);
    }

    SECTION("selects first item when executed if none was save")
    {
        SettingsPageRegistry::deleteAllPages();

        auto page1 =
            std::make_shared<MockSettingsPage>(HierarchicalId("MockTestPage"));
        auto page2 =
            std::make_shared<MockSettingsPage>(HierarchicalId("MockTestPage2"));

        SettingsPageRegistry::addPage(page1);
        SettingsPageRegistry::addPage(page2);

        useCase.showSettingsDialog();

        REQUIRE(view->getSelectedGroupIndex().row() == 0);
        REQUIRE(view->getSelectedGroupIndex().column() == 0);
        REQUIRE(view->getSelectedGroupIndex().parent() == QModelIndex());
    }

    SECTION("selects last selected when executed if it was saved")
    {
        SettingsPageRegistry::deleteAllPages();

        auto page1 =
            std::make_shared<MockSettingsPage>(HierarchicalId("MockTestPage"));
        auto page2 = std::make_shared<MockSettingsPage>(
            HierarchicalId("MockTestPage2")("Subpage1")("Subpage2"));

        SettingsPageRegistry::addPage(page1);
        SettingsPageRegistry::addPage(page2);

        QString dataToBeSaved{QString::fromStdString(page2->group().name())};

        settings.setValue(KEYS().UI.SETTINGS_DIALOG_TREE_VIEW_SELECTED_ITEM_KEY,
                          dataToBeSaved);

        useCase.showSettingsDialog();

        REQUIRE(view->getSelectedGroupIndex().row() == 0);
        REQUIRE(view->getSelectedGroupIndex().column() == 0);
        REQUIRE(view->getSelectedGroupIndex().parent().row() == 0);
        REQUIRE(view->getSelectedGroupIndex().parent().column() == 0);
        REQUIRE(view->getSelectedGroupIndex().parent().parent().row() == 1);
        REQUIRE(view->getSelectedGroupIndex().parent().parent().column() == 0);
        REQUIRE(view->getSelectedGroupIndex().parent().parent().parent() ==
                QModelIndex());
    }

    SettingsPageRegistry::deleteAllPages();
}

TEST_CASE("Any settings dialog", "[Issue 36]")
{
    const auto view = std::make_shared<MockSettingsDialog>();
    MockSettings settings;
    const auto logger = std::make_shared<NullLogger>();
    ShowSettingsDialog useCase{view, settings, logger};

    SECTION(
        "shall not crash if anyGuiElementChanged is called with empty page ")
    {
        REQUIRE_NOTHROW(useCase.anyGuiElementHasChanged());
    }
}
