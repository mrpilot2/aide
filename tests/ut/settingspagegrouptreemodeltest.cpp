#include <catch2/catch.hpp>

#include "hierarchicalid.hpp"
#include "mocksettingspage.hpp"
#include "settings/settingspagegrouptreemodel.hpp"
#include "settings/settingspageregistry.hpp"

using aide::HierarchicalId;
using aide::core::SettingsPageGroupTreeModel;
using aide::core::SettingsPageRegistry;
using aide::test::MockSettingsPage;

TEST_CASE("A new settings page group tree model without registered page")
{
    SettingsPageRegistry::deleteAllPages();
    SettingsPageGroupTreeModel treeModel;

    SECTION("has no rows") { REQUIRE(treeModel.rowCount() == 0); }
}

TEST_CASE("A new settings page group tree model with one registered page")
{
    SettingsPageRegistry::deleteAllPages();

    SettingsPageRegistry::addPage(
        std::make_unique<MockSettingsPage>(HierarchicalId("MockTestPage")));

    SettingsPageGroupTreeModel treeModel;

    SECTION("has one row") { REQUIRE(treeModel.rowCount() == 1); }

    SECTION("has one column") { REQUIRE(treeModel.columnCount() == 1); }

    SECTION("provides group in first column and first row")
    {
        QModelIndex index(treeModel.index(0, 0));
        REQUIRE(
            treeModel.data(index, Qt::DisplayRole).toString().toStdString() ==
            "MockTestPage");
    }

    SECTION("does not have parent in first row")
    {
        QModelIndex index(treeModel.index(0, 0));

        auto parentIndex = treeModel.parent(index);
        REQUIRE(!parentIndex.isValid());
    }
}

TEST_CASE("A new settings page group tree model with multiple registered page")
{
    SettingsPageRegistry::deleteAllPages();

    SettingsPageRegistry::addPage(std::make_unique<MockSettingsPage>(
        HierarchicalId("MockTestPage")("Subpage1")));
    SettingsPageRegistry::addPage(std::make_unique<MockSettingsPage>(
        HierarchicalId("MockTestPage")("Subpage2")));

    SettingsPageRegistry::addPage(
        std::make_unique<MockSettingsPage>(HierarchicalId("MockTestPage2")));

    SettingsPageGroupTreeModel treeModel;

    SECTION("use same tree item for same group")
    {
        REQUIRE(treeModel.rowCount() == 2);
    }

    SECTION("creates a valid child model index")
    {
        auto parentIndex = treeModel.index(0, 0);

        auto childIndex = treeModel.index(0, 0, parentIndex);

        REQUIRE(treeModel.data(childIndex, Qt::DisplayRole)
                    .toString()
                    .toStdString() == "Subpage1");
    }

    SECTION("creates a valid parent index")
    {
        auto parentIndex = treeModel.index(0, 0);

        auto childIndex = treeModel.index(1, 0, parentIndex);

        REQUIRE(treeModel.parent(childIndex) == parentIndex);
    }
    SECTION("returns invalid model index if child does not exist")
    {
        auto parentIndex = treeModel.index(0, 0);

        auto childIndex = treeModel.index(5, 0, parentIndex);

        REQUIRE(!childIndex.isValid());
    }
}

TEST_CASE("Any settings page group tree model")
{
    SettingsPageRegistry::deleteAllPages();

    SettingsPageRegistry::addPage(
        std::make_unique<MockSettingsPage>(HierarchicalId("MockTestPage")));

    SettingsPageGroupTreeModel treeModel;

    SECTION("provides horizontal header")
    {
        REQUIRE(treeModel.headerData(0, Qt::Horizontal, Qt::DisplayRole)
                    .toString()
                    .toStdString() == "Group");
    }

    SECTION("does not provide vertical header")
    {
        REQUIRE(
            !treeModel.headerData(0, Qt::Vertical, Qt::DisplayRole).isValid());
    }

    SECTION("has selectable items")
    {
        QModelIndex index = treeModel.index(0, 0);

        REQUIRE(treeModel.flags(index).testFlag(Qt::ItemIsSelectable));
    }

    SECTION("removes item flags for invalid index")
    {
        QModelIndex index = treeModel.index(5, 0);

        REQUIRE(treeModel.flags(index).testFlag(Qt::NoItemFlags));
    }
}
