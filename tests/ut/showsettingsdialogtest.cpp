#include <array>
#include <stdexcept>

#include <catch2/catch_test_macros.hpp>

#include <QItemSelection>
#include <QLabel>
#include <QStandardItemModel>

#include "commonsettingskeys.hpp"
#include "hierarchicalid.hpp"
#include "mocksettings.hpp"
#include "mocksettingsdialog.hpp"
#include "mocksettingspage.hpp"
#include "nulllogger.hpp"
#include "settings/searchhistory.hpp"
#include "settings/settingspagegrouptreemodel.hpp"
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
    SettingsPageRegistry registry;
    ShowSettingsDialog useCase{view, registry, settings, logger};

    SECTION("shows settings dialog if requested by user")
    {
        useCase.showSettingsDialog();

        REQUIRE(view->wasSettingsDialogExecuted());
    }

    SECTION("works with base class pointer")
    {
        std::unique_ptr<ShowSettingsDialogController> base =
            std::make_unique<ShowSettingsDialog>(view, registry, settings,
                                                 logger);

        base->showSettingsDialog();

        REQUIRE(view->wasSettingsDialogExecuted());
    }

    SECTION("show display name of selected group")
    {
        registry.addPage(std::make_unique<MockSettingsPage>(
            HierarchicalId("MockTestPage")("Subpage1")));

        const SettingsPageGroupTreeModel treeModel{registry};

        useCase.showSettingsDialog();

        useCase.changeSelectedPage(
            QItemSelection(treeModel.index(0, 0, QModelIndex()),
                           treeModel.index(0, 0, QModelIndex())),
            QItemSelection(treeModel.index(-1, -1, QModelIndex()),
                           treeModel.index(-1, -1, QModelIndex())));

        REQUIRE(view->displayName() == "MockTestPage");
    }

    SECTION("clears the page without throwing when the selection is emptied")
    {
        registry.addPage(
            std::make_unique<MockSettingsPage>(HierarchicalId("MockTestPage")));

        const SettingsPageGroupTreeModel treeModel{registry};

        useCase.showSettingsDialog();

        const QItemSelection pageSelection(
            treeModel.index(0, 0, QModelIndex()),
            treeModel.index(0, 0, QModelIndex()));

        useCase.changeSelectedPage(pageSelection, QItemSelection());

        // Filtering removes the selected row, so the selection model emits
        // selectionChanged() with an empty selection. This must not abort.
        REQUIRE_NOTHROW(
            useCase.changeSelectedPage(QItemSelection(), pageSelection));
    }

    SECTION("can be used with base class interface to change page")
    {
        auto base =
            std::shared_ptr<aide::core::SettingsDialogChangePageController>(
                std::make_shared<ShowSettingsDialog>(view, registry, settings,
                                                     logger));

        registry.addPage(
            std::make_unique<MockSettingsPage>(HierarchicalId("MockTestPage")));

        const SettingsPageGroupTreeModel treeModel{registry};

        REQUIRE_THROWS_AS(
            base->changeSelectedPage(
                QItemSelection(treeModel.index(0, 0, QModelIndex()),
                               treeModel.index(0, 0, QModelIndex())),
                QItemSelection(treeModel.index(-1, -1, QModelIndex()),
                               treeModel.index(-1, -1, QModelIndex()))),
            std::logic_error);
    }

    SECTION("stores search pattern forwarded from the dialog")
    {
        useCase.showSettingsDialog();

        useCase.searchPatternChanged("needle");

        REQUIRE(useCase.currentSearchPattern() == "needle");
    }

    SECTION("commits the current search pattern to history on demand")
    {
        useCase.searchPatternChanged("font");

        useCase.commitCurrentSearchPattern();

        const aide::core::SearchHistory history{settings};
        REQUIRE(history.entries() == QStringList{"font"});
    }

    SECTION("commits an active non-empty search pattern when closing")
    {
        useCase.searchPatternChanged("color");

        useCase.showSettingsDialog();

        const aide::core::SearchHistory history{settings};
        REQUIRE(history.entries() == QStringList{"color"});
    }

    SECTION("does not commit an empty search pattern when closing")
    {
        useCase.showSettingsDialog();

        const aide::core::SearchHistory history{settings};
        REQUIRE(history.entries().isEmpty());
    }

    SECTION("highlights the current page when the search pattern changes")
    {
        auto page = std::make_shared<MockSettingsPage>(
            HierarchicalId("MockTestPage"), "Enable dark mode");
        registry.addPage(page);

        const SettingsPageGroupTreeModel treeModel{registry};

        useCase.showSettingsDialog();

        useCase.changeSelectedPage(
            QItemSelection(treeModel.index(0, 0, QModelIndex()),
                           treeModel.index(0, 0, QModelIndex())),
            QItemSelection(treeModel.index(-1, -1, QModelIndex()),
                           treeModel.index(-1, -1, QModelIndex())));

        useCase.searchPatternChanged("dark");

        auto* label = page->widget()->findChild<QLabel*>();
        REQUIRE(label != nullptr);
        REQUIRE(label->styleSheet().contains("#8B4513"));
    }

    SECTION("highlights a newly selected page while a search is active")
    {
        auto page = std::make_shared<MockSettingsPage>(
            HierarchicalId("MockTestPage"), "Enable dark mode");
        registry.addPage(page);

        const SettingsPageGroupTreeModel treeModel{registry};

        useCase.showSettingsDialog();

        useCase.searchPatternChanged("dark");

        useCase.changeSelectedPage(
            QItemSelection(treeModel.index(0, 0, QModelIndex()),
                           treeModel.index(0, 0, QModelIndex())),
            QItemSelection(treeModel.index(-1, -1, QModelIndex()),
                           treeModel.index(-1, -1, QModelIndex())));

        auto* label = page->widget()->findChild<QLabel*>();
        REQUIRE(label != nullptr);
        REQUIRE(label->styleSheet().contains("#8B4513"));
    }

    SECTION("auto-selects the page whose content best matches the search")
    {
        // "Alpha" is registered first (tree row 0) but only buries the word;
        // "Beta" (row 1) has the exact-word match and should win.
        registry.addPage(std::make_unique<MockSettingsPage>(
            HierarchicalId("Alpha"), "Change font size"));
        registry.addPage(std::make_unique<MockSettingsPage>(
            HierarchicalId("Beta"), "Font:"));

        useCase.showSettingsDialog();

        // The dialog opens on the first page.
        REQUIRE(view->getSelectedGroupIndex().row() == 0);

        useCase.searchPatternChanged("font");

        REQUIRE(view->getSelectedGroupIndex().row() == 1);
        REQUIRE(view->getSelectedGroupIndex().parent() == QModelIndex());
    }

    SECTION("ranks a multi-word search by the sum of best per-word scores")
    {
        // "Alpha" scores high on one word only ("Font:" -> 0.8); "Beta"
        // scores on both words across two widgets and so wins on the sum.
        registry.addPage(std::make_unique<MockSettingsPage>(
            HierarchicalId("Alpha"), "Font:"));
        auto beta = std::make_shared<MockSettingsPage>(HierarchicalId("Beta"),
                                                       "Font family");
        (void)new QLabel("Size:", beta->widget());
        registry.addPage(beta);

        useCase.showSettingsDialog();

        useCase.searchPatternChanged("font size");

        REQUIRE(view->getSelectedGroupIndex().row() == 1);
    }

    SECTION("does not auto-select a prior search's best page on reopen")
    {
        registry.addPage(std::make_unique<MockSettingsPage>(
            HierarchicalId("Alpha"), "Change font size"));
        registry.addPage(std::make_unique<MockSettingsPage>(
            HierarchicalId("Beta"), "Font:"));

        useCase.showSettingsDialog();

        useCase.searchPatternChanged("font");
        REQUIRE(view->getSelectedGroupIndex().row() == 1);

        // The dialog reopens with an empty search field.
        useCase.searchPatternChanged("");
        useCase.showSettingsDialog();

        // Reopening starts fresh: the first page is selected, not the page a
        // prior search had auto-selected.
        REQUIRE(view->getSelectedGroupIndex().row() == 0);
    }

    SECTION("show selected page widget")
    {
        registry.addPage(
            std::make_unique<MockSettingsPage>(HierarchicalId("MockTestPage")));

        const SettingsPageGroupTreeModel treeModel{registry};

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
        auto page =
            std::make_shared<MockSettingsPage>(HierarchicalId("MockTestPage"));
        registry.addPage(page);

        const SettingsPageGroupTreeModel treeModel{registry};

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
        auto page =
            std::make_shared<MockSettingsPage>(HierarchicalId("MockTestPage"));
        registry.addPage(page);

        const SettingsPageGroupTreeModel treeModel{registry};

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
        auto page =
            std::make_shared<MockSettingsPage>(HierarchicalId("MockTestPage"));
        registry.addPage(page);

        const SettingsPageGroupTreeModel treeModel{registry};

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
        auto page1 =
            std::make_shared<MockSettingsPage>(HierarchicalId("MockTestPage"));
        auto page2 =
            std::make_shared<MockSettingsPage>(HierarchicalId("MockTestPage2"));

        registry.addPage(page1);
        registry.addPage(page2);

        const SettingsPageGroupTreeModel treeModel{registry};

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
        auto page1 =
            std::make_shared<MockSettingsPage>(HierarchicalId("MockTestPage"));
        auto page2 =
            std::make_shared<MockSettingsPage>(HierarchicalId("MockTestPage2"));

        registry.addPage(page1);
        registry.addPage(page2);

        const SettingsPageGroupTreeModel treeModel{registry};

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
        auto page1 =
            std::make_shared<MockSettingsPage>(HierarchicalId("MockTestPage"));
        auto page2 =
            std::make_shared<MockSettingsPage>(HierarchicalId("MockTestPage2"));

        registry.addPage(page1);
        registry.addPage(page2);

        const SettingsPageGroupTreeModel treeModel{registry};

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
        auto page1 =
            std::make_shared<MockSettingsPage>(HierarchicalId("MockTestPage"));
        auto page2 =
            std::make_shared<MockSettingsPage>(HierarchicalId("MockTestPage2"));

        registry.addPage(page1);
        registry.addPage(page2);

        page1->simulateModified(false);
        page2->simulateModified(true);

        view->simulateUserAcceptsDialog(true);

        useCase.showSettingsDialog();

        REQUIRE(!page1->wasApplyCalled());
        REQUIRE(page2->wasApplyCalled());
    }

    SECTION("resets currently shown page if user presses reset label")
    {
        auto page1 =
            std::make_shared<MockSettingsPage>(HierarchicalId("MockTestPage"));
        auto page2 =
            std::make_shared<MockSettingsPage>(HierarchicalId("MockTestPage2"));

        registry.addPage(page1);
        registry.addPage(page2);

        const SettingsPageGroupTreeModel treeModel{registry};

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
        auto page1 =
            std::make_shared<MockSettingsPage>(HierarchicalId("MockTestPage"));
        auto page2 =
            std::make_shared<MockSettingsPage>(HierarchicalId("MockTestPage2"));

        registry.addPage(page1);
        registry.addPage(page2);

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
        auto page1 =
            std::make_shared<MockSettingsPage>(HierarchicalId("MockTestPage"));
        auto page2 =
            std::make_shared<MockSettingsPage>(HierarchicalId("MockTestPage2"));

        registry.addPage(page1);
        registry.addPage(page2);

        useCase.showSettingsDialog();

        REQUIRE(view->getSelectedGroupIndex().row() == 0);
        REQUIRE(view->getSelectedGroupIndex().column() == 0);
        REQUIRE(view->getSelectedGroupIndex().parent() == QModelIndex());
    }

    SECTION("selects last selected when executed if it was saved")
    {
        auto page1 =
            std::make_shared<MockSettingsPage>(HierarchicalId("MockTestPage"));
        auto page2 = std::make_shared<MockSettingsPage>(
            HierarchicalId("MockTestPage2")("Subpage1")("Subpage2"));

        registry.addPage(page1);
        registry.addPage(page2);

        const QString dataToBeSaved{
            QString::fromStdString(page2->group().name())};

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
}

TEST_CASE(
    "ShowSettingsDialog changeSelectedPage before showSettingsDialog is called")
{
    auto view = std::make_shared<MockSettingsDialog>();
    MockSettings settings;
    auto logger = std::make_shared<NullLogger>();
    SettingsPageRegistry registry;
    ShowSettingsDialog useCase{view, registry, settings, logger};

    SECTION("throws std::logic_error when tree model is null")
    {
        QStandardItemModel tempModel;
        tempModel.appendRow(new QStandardItem("item"));
        const auto validIndex = tempModel.index(0, 0);
        const QItemSelection nonEmptySelection(validIndex, validIndex);

        REQUIRE_THROWS_AS(
            useCase.changeSelectedPage(nonEmptySelection, QItemSelection()),
            std::logic_error);
    }
}

TEST_CASE("Any settings dialog", "[Issue 36]")
{
    const auto view = std::make_shared<MockSettingsDialog>();
    MockSettings settings;
    const auto logger = std::make_shared<NullLogger>();
    SettingsPageRegistry registry;
    ShowSettingsDialog useCase{view, registry, settings, logger};

    SECTION(
        "shall not crash if anyGuiElementChanged is called with empty page ")
    {
        REQUIRE_NOTHROW(useCase.anyGuiElementHasChanged());
    }
}
