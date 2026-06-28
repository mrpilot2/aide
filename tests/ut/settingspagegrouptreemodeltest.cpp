#include <catch2/catch_test_macros.hpp>

#include <QColor>

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
    SettingsPageRegistry registry;

    SECTION("has no rows")
    {
        const SettingsPageGroupTreeModel treeModel{registry};

        REQUIRE(treeModel.rowCount() == 0);
    }
}

TEST_CASE("A new settings page group tree model with one registered page")
{
    SettingsPageRegistry registry;
    registry.addPage(
        std::make_unique<MockSettingsPage>(HierarchicalId("MockTestPage")));

    const SettingsPageGroupTreeModel treeModel{registry};

    SECTION("has one row")
    {
        REQUIRE(treeModel.rowCount() == 1);
    }

    SECTION("has one column")
    {
        REQUIRE(treeModel.columnCount() == 1);
    }

    SECTION("provides group in first column and first row")
    {
        const QModelIndex index(treeModel.index(0, 0));
        REQUIRE(
            treeModel.data(index, Qt::DisplayRole).toString().toStdString() ==
            "MockTestPage");
    }

    SECTION("does not have parent in first row")
    {
        const QModelIndex index(treeModel.index(0, 0));

        const auto parentIndex = treeModel.parent(index);
        REQUIRE(!parentIndex.isValid());
    }
}

TEST_CASE("A new settings page group tree model with multiple registered page")
{
    SettingsPageRegistry registry;
    registry.addPage(std::make_unique<MockSettingsPage>(
        HierarchicalId("MockTestPage")("Subpage1")));
    registry.addPage(std::make_unique<MockSettingsPage>(
        HierarchicalId("MockTestPage")("Subpage2")));
    registry.addPage(
        std::make_unique<MockSettingsPage>(HierarchicalId("MockTestPage2")));

    const SettingsPageGroupTreeModel treeModel{registry};

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

        constexpr auto rowIndexTooHigh{5};
        auto childIndex = treeModel.index(rowIndexTooHigh, 0, parentIndex);

        REQUIRE(!childIndex.isValid());
    }
}

TEST_CASE("Any settings page group tree model")
{
    SettingsPageRegistry registry;
    auto mockPage =
        std::make_shared<MockSettingsPage>(HierarchicalId("MockTestPage"));
    registry.addPage(mockPage);

    const SettingsPageGroupTreeModel treeModel{registry};

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
        const QModelIndex index = treeModel.index(0, 0);

        REQUIRE(treeModel.flags(index).testFlag(Qt::ItemIsSelectable));
    }

    SECTION("removes item flags for invalid index")
    {
        constexpr auto rowIndexTooHigh{5};
        const QModelIndex index = treeModel.index(rowIndexTooHigh, 0);

        REQUIRE(treeModel.flags(index).testFlag(Qt::NoItemFlags));
    }

    SECTION("highlights modified settings page")
    {
        mockPage->simulateModified(true);

        const QModelIndex index = treeModel.index(0, 0);

        REQUIRE(treeModel.data(index, Qt::ForegroundRole) == QColor(Qt::blue));
    }

    SECTION("does not highlight unmodified settings page")
    {
        mockPage->simulateModified(false);

        const QModelIndex index = treeModel.index(0, 0);

        REQUIRE(treeModel.data(index, Qt::ForegroundRole) == QVariant());
    }

    SECTION("can recursively be search for a ModelIndex by group name")
    {
        SettingsPageRegistry extraRegistry;
        auto subSubPage1 = std::make_shared<MockSettingsPage>(
            HierarchicalId("MockTestPage")("SubPage")("SubSubPage1"));
        auto subSubPage2 = std::make_shared<MockSettingsPage>(
            HierarchicalId("MockTestPage")("SubPage")("SubSubPage2"));

        extraRegistry.addPage(subSubPage1);
        extraRegistry.addPage(subSubPage2);

        const SettingsPageGroupTreeModel recursiveTreeModel{extraRegistry};

        auto root          = recursiveTreeModel.index(0, 0, QModelIndex());
        auto subPage       = recursiveTreeModel.index(0, 0, root);
        auto expectedIndex = recursiveTreeModel.index(1, 0, subPage);

        REQUIRE(recursiveTreeModel.recursivelyFindSelectedTreeItemIndex(
                    QString::fromStdString(subSubPage2->group().name()),
                    QModelIndex()) == expectedIndex);
    }

    SECTION("can recursively be search for a ModelIndex by group name test 2")
    {
        SettingsPageRegistry extraRegistry;
        auto subSubPage1 =
            std::make_shared<MockSettingsPage>(HierarchicalId("MockTestPage"));
        auto subSubPage2 = std::make_shared<MockSettingsPage>(
            HierarchicalId("MockTestPage2")("SubPage")("SubSubPage2"));

        extraRegistry.addPage(subSubPage1);
        extraRegistry.addPage(subSubPage2);

        const SettingsPageGroupTreeModel recursiveTreeModel{extraRegistry};

        auto root          = recursiveTreeModel.index(1, 0, QModelIndex());
        auto subPage       = recursiveTreeModel.index(0, 0, root);
        auto expectedIndex = recursiveTreeModel.index(0, 0, subPage);

        REQUIRE(recursiveTreeModel.recursivelyFindSelectedTreeItemIndex(
                    QString::fromStdString(subSubPage2->group().name()),
                    QModelIndex()) == expectedIndex);
    }
}
