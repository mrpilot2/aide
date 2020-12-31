#include <stdexcept>

#include <catch2/catch.hpp>

#include <QItemSelection>

#include "hierarchicalid.hpp"
#include "mocksettingsdialog.hpp"
#include "mocksettingspage.hpp"
#include "nulllogger.hpp"
#include "settings/settingspageregistry.hpp"
#include "settings/showsettingsdialog.hpp"
#include "settings/showsettingsdialogcontroller.hpp"

using aide::HierarchicalId;
using aide::core::SettingsPageRegistry;
using aide::core::ShowSettingsDialog;
using aide::core::ShowSettingsDialogController;
using aide::test::MockSettingsDialog;
using aide::test::MockSettingsPage;
using aide::test::NullLogger;

TEST_CASE("Any show settings dialog use case")
{
    auto view   = std::make_shared<MockSettingsDialog>();
    auto logger = std::make_shared<NullLogger>();
    ShowSettingsDialog useCase{view, logger};

    SECTION("shows settings dialog if requested by user")
    {
        useCase.showSettingsDialog();

        REQUIRE(view->wasSettingsDialogExecuted());
    }

    SECTION("works with base class pointer")
    {
        std::unique_ptr<ShowSettingsDialogController> base =
            std::make_unique<ShowSettingsDialog>(view, logger);

        base->showSettingsDialog();

        REQUIRE(view->wasSettingsDialogExecuted());
    }

    SECTION("show display name of selected group")
    {
        SettingsPageRegistry::deleteAllPages();

        SettingsPageRegistry::addPage(std::make_unique<MockSettingsPage>(
            HierarchicalId("MockTestPage")("Subpage1")));

        aide::core::SettingsPageGroupTreeModel treeModel;

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

        aide::core::SettingsPageGroupTreeModel treeModel;

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
                std::make_shared<ShowSettingsDialog>(view, logger));

        aide::core::SettingsPageGroupTreeModel treeModel;

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

        aide::core::SettingsPageGroupTreeModel treeModel;

        useCase.showSettingsDialog();

        REQUIRE(view->currentlyShownWidget() == nullptr);

        useCase.changeSelectedPage(
            QItemSelection(treeModel.index(0, 0, QModelIndex()),
                           treeModel.index(0, 0, QModelIndex())),
            QItemSelection(treeModel.index(-1, -1, QModelIndex()),
                           treeModel.index(-1, -1, QModelIndex())));

        REQUIRE(view->currentlyShownWidget() != nullptr);
    }
}
