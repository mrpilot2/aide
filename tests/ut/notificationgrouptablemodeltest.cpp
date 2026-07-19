#include <vector>

#include <catch2/catch_test_macros.hpp>

#include <QColor>

#include <aide/hierarchicalid.hpp>
#include <aide/notificationdisplaytype.hpp>
#include <aide/notificationgroup.hpp>

#include "settings/notifications/notificationgrouptablemodel.hpp"

using aide::HierarchicalId;
using aide::NotificationDisplayType;
using aide::NotificationGroup;
using aide::core::NotificationGroupTableModel;

namespace
{
    std::vector<NotificationGroup> twoGroups()
    {
        return {NotificationGroup{
                    .id                 = HierarchicalId("build"),
                    .displayName        = "Build",
                    .defaultDisplayType = NotificationDisplayType::Balloon},
                NotificationGroup{
                    .id                 = HierarchicalId("vcs"),
                    .displayName        = "VCS",
                    .defaultDisplayType = NotificationDisplayType::None}};
    }
} // namespace

TEST_CASE("NotificationGroupTableModel with no groups",
          "[NotificationGroupTableModel]")
{
    const NotificationGroupTableModel model;

    REQUIRE(model.rowCount() == 0);
    REQUIRE(model.columnCount() == NotificationGroupTableModel::ColumnCount);
}

TEST_CASE("NotificationGroupTableModel seeded with groups",
          "[NotificationGroupTableModel]")
{
    NotificationGroupTableModel model;
    model.setGroups(twoGroups());

    SECTION("has one row per group")
    {
        REQUIRE(model.rowCount() == 2);
    }

    SECTION("seeds each row's display type from the group default")
    {
        REQUIRE(model.displayTypeAt(0) == NotificationDisplayType::Balloon);
        REQUIRE(model.displayTypeAt(1) == NotificationDisplayType::None);
    }

    SECTION("shows the group's display name and id")
    {
        const auto index =
            model.index(0, NotificationGroupTableModel::GroupColumn);
        REQUIRE(model.data(index, Qt::DisplayRole).toString() ==
                "Build (build)");
    }

    SECTION("shows the group id as a tooltip")
    {
        const auto index =
            model.index(0, NotificationGroupTableModel::GroupColumn);
        REQUIRE(model.data(index, Qt::ToolTipRole).toString() == "build");
    }

    SECTION("popup type column carries the display type as edit data")
    {
        const auto index =
            model.index(0, NotificationGroupTableModel::PopupTypeColumn);
        REQUIRE(model.data(index, Qt::EditRole).toInt() ==
                static_cast<int>(NotificationDisplayType::Balloon));
    }

    SECTION("log column always reads On")
    {
        const auto index =
            model.index(0, NotificationGroupTableModel::LogColumn);
        REQUIRE(model.data(index, Qt::DisplayRole).toString() == "On");
    }

    SECTION("only the popup type column is editable")
    {
        const auto groupFlags = model.flags(
            model.index(0, NotificationGroupTableModel::GroupColumn));
        const auto popupFlags = model.flags(
            model.index(0, NotificationGroupTableModel::PopupTypeColumn));
        const auto logFlags =
            model.flags(model.index(0, NotificationGroupTableModel::LogColumn));

        REQUIRE_FALSE(groupFlags & Qt::ItemIsEditable);
        REQUIRE(popupFlags & Qt::ItemIsEditable);
        REQUIRE_FALSE(logFlags & Qt::ItemIsEditable);
    }

    SECTION("setDisplayTypeAt updates the row and emits dataChanged")
    {
        model.setDisplayTypeAt(1, NotificationDisplayType::StickyBalloon);
        REQUIRE(model.displayTypeAt(1) ==
                NotificationDisplayType::StickyBalloon);
    }

    SECTION("setData on the popup type column updates the row")
    {
        const auto index =
            model.index(1, NotificationGroupTableModel::PopupTypeColumn);
        REQUIRE(model.setData(
            index, static_cast<int>(NotificationDisplayType::StickyBalloon),
            Qt::EditRole));
        REQUIRE(model.displayTypeAt(1) ==
                NotificationDisplayType::StickyBalloon);
    }

    SECTION("setData on a non-editable column is rejected")
    {
        const auto index =
            model.index(0, NotificationGroupTableModel::GroupColumn);
        REQUIRE_FALSE(model.setData(index, "Something", Qt::EditRole));
    }

    SECTION("groupAt returns the group for the row")
    {
        REQUIRE(model.groupAt(0).id == HierarchicalId("build"));
        REQUIRE(model.groupAt(1).id == HierarchicalId("vcs"));
    }

    SECTION("matchesAnyRow finds a group by id or display name")
    {
        REQUIRE(model.matchesAnyRow({"build"}));
        REQUIRE(model.matchesAnyRow({"VCS"}));
        REQUIRE_FALSE(model.matchesAnyRow({"nonexistent"}));
    }

    SECTION("rowsScore is zero when nothing matches")
    {
        REQUIRE(model.rowsScore({"nonexistent"}) == 0.0);
    }

    SECTION("rowsScore is positive when a word matches a row")
    {
        REQUIRE(model.rowsScore({"Build"}) > 0.0);
    }

    SECTION("setSearchPattern highlights matching rows")
    {
        model.setSearchPattern("build");

        const auto matchingIndex =
            model.index(0, NotificationGroupTableModel::GroupColumn);
        const auto nonMatchingIndex =
            model.index(1, NotificationGroupTableModel::GroupColumn);

        REQUIRE(model.data(matchingIndex, Qt::BackgroundRole).isValid());
        REQUIRE_FALSE(
            model.data(nonMatchingIndex, Qt::BackgroundRole).isValid());
    }

    SECTION("an empty search pattern clears the highlight")
    {
        model.setSearchPattern("build");
        model.setSearchPattern("");

        const auto index =
            model.index(0, NotificationGroupTableModel::GroupColumn);
        REQUIRE_FALSE(model.data(index, Qt::BackgroundRole).isValid());
    }
}

TEST_CASE("NotificationGroupTableModel::displayTypeLabel",
          "[NotificationGroupTableModel]")
{
    REQUIRE_FALSE(NotificationGroupTableModel::displayTypeLabel(
                      NotificationDisplayType::None)
                      .isEmpty());
    REQUIRE_FALSE(NotificationGroupTableModel::displayTypeLabel(
                      NotificationDisplayType::Balloon)
                      .isEmpty());
    REQUIRE_FALSE(NotificationGroupTableModel::displayTypeLabel(
                      NotificationDisplayType::StickyBalloon)
                      .isEmpty());
}
