#include <catch2/catch_test_macros.hpp>

#include <QTest>

#include <aide/gui/widgets/banner.hpp>

#include "notificationdemodialog.hpp"
#include "testhelpers.hpp"

using aide::widgets::Banner;
using demo::NotificationDemoDialog;
using demo::test::findButton;

TEST_CASE("A NotificationDemoDialog", "[NotificationDemoDialog]")
{
    const NotificationDemoDialog dialog;

    SECTION("has Info, Success, Warning, Error and Clear buttons")
    {
        REQUIRE(findButton(dialog, "Info") != nullptr);
        REQUIRE(findButton(dialog, "Success") != nullptr);
        REQUIRE(findButton(dialog, "Warning") != nullptr);
        REQUIRE(findButton(dialog, "Error") != nullptr);
        REQUIRE(findButton(dialog, "Clear") != nullptr);
    }

    SECTION("a severity button shows a banner")
    {
        QTest::mouseClick(findButton(dialog, "Info"), Qt::LeftButton);

        REQUIRE(dialog.findChildren<Banner*>().size() == 1);
    }

    SECTION("clicking a second severity button replaces the first banner")
    {
        QTest::mouseClick(findButton(dialog, "Info"), Qt::LeftButton);
        QTest::mouseClick(findButton(dialog, "Error"), Qt::LeftButton);

        REQUIRE(dialog.findChildren<Banner*>().size() == 1);
    }

    SECTION("Clear removes the currently shown banner")
    {
        QTest::mouseClick(findButton(dialog, "Warning"), Qt::LeftButton);
        QTest::mouseClick(findButton(dialog, "Clear"), Qt::LeftButton);

        REQUIRE(dialog.findChildren<Banner*>().isEmpty());
    }
}
