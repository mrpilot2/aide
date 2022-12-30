#include <array>

#include <QApplication>
#include <QIcon>
#include <QLineEdit>
#include <QMainWindow>
#include <QSignalSpy>
#include <QToolButton>
#include <QtTest/qtestkeyboard.h>

#include <aide/hierarchicalid.hpp>

#include "aide/gui/widgets/searchlineedit.hpp"
#include "catch2/catch.hpp"

using aide::widgets::SearchLineEdit;

TEST_CASE("Any search line edit ")
{
    int numberOfArgs{1};
    // NOLINTNEXTLINE
    std::array<char*, 1> appName{{const_cast<char*>("aide_test")}};
    QApplication app(numberOfArgs, appName.data());

    QMainWindow mainWindow;

    Q_INIT_RESOURCE(ut_icons);

    SearchLineEdit searchLineEdit(aide::HierarchicalId("test"),
                                  QKeySequence(Qt::Key_F), &mainWindow);

    SECTION(" allows empty search icon")
    {
        searchLineEdit.setSearchIcon(QIcon());

        const auto* child =
            searchLineEdit.findChild<QToolButton*>("toolButton");

        REQUIRE(child->icon().isNull());
    }

    SECTION(" displays a custom search icon")
    {
        searchLineEdit.setSearchIcon(QIcon(":/icons/unit-test/system-search"));

        const auto* child =
            searchLineEdit.findChild<QToolButton*>("toolButton");

        REQUIRE(!child->icon().isNull());

        REQUIRE(16 == child->iconSize().width());
        REQUIRE(16 == child->iconSize().height());
    }

    SECTION(" emit the textChanged signal if text in the search box is entered")
    {
        auto* child = searchLineEdit.findChild<QLineEdit*>("searchField");

        QSignalSpy spy(child, SIGNAL(textChanged(const QString&)));

        QTest::keyClicks(child, "hello world");

        REQUIRE(spy.count() > 0);
    }
}
