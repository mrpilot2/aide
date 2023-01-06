#include <array>

#include <QIcon>
#include <QLineEdit>
#include <QMainWindow>
#include <QSignalSpy>
#include <QToolButton>
#include <QtTest/qtestkeyboard.h>

#include <aide/application.hpp>
#include <aide/hierarchicalid.hpp>

#include "aide/gui/widgets/searchfilterwidget.hpp"
#include "catch2/catch.hpp"

using aide::widgets::SearchFilterWidget;

TEST_CASE("Any search filter widget ")
{
    aide::Application::setApplicationName("test");
    aide::Application::setOrganizationName("org");
    int numberOfArgs{1};
    // NOLINTNEXTLINE
    std::array<char*, 1> appName{{const_cast<char*>("aide_test")}};
    QApplication app(numberOfArgs, appName.data());

    Q_INIT_RESOURCE(ut_icons);

    QMainWindow* mainWindow = new QMainWindow();
    SearchFilterWidget searchFilterWidget(aide::HierarchicalId("test"),
                                          QKeySequence(Qt::Key_F), mainWindow);

    SECTION(" allows empty search icon")
    {
        searchFilterWidget.setSearchIcon(QIcon());

        const auto* child =
            searchFilterWidget.findChild<QToolButton*>("toolButton");

        REQUIRE(child->icon().isNull());
    }

    SECTION(" displays a custom search icon")
    {
        searchFilterWidget.setSearchIcon(
            QIcon(":/icons/unit-test/system-search"));

        const auto* child =
            searchFilterWidget.findChild<QToolButton*>("toolButton");

        REQUIRE(!child->icon().isNull());

        REQUIRE(16 == child->iconSize().width());
        REQUIRE(16 == child->iconSize().height());
    }

    SECTION(" emit the textChanged signal if text in the search box is entered")
    {
        auto* child = searchFilterWidget.findChild<QLineEdit*>("searchField");

        QSignalSpy spy(child, SIGNAL(textChanged(const QString&)));

        QTest::keyClicks(child, "hello world");

        REQUIRE(spy.count() > 0);
    }
}
