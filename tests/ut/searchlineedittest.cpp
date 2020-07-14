#include <array>

#include <QApplication>
#include <QIcon>
#include <QLabel>
#include <QLineEdit>
#include <QSignalSpy>
#include <QtTest/qtestkeyboard.h>

#include "aide/gui/widgets/searchlineedit.hpp"
#include "catch2/catch.hpp"

using aide::gui::SearchLineEdit;

TEST_CASE("Any search line edit ")
{
    int numberOfArgs{1};
    // NOLINTNEXTLINE
    std::array<char*, 1> appName{{const_cast<char*>("aide_test")}};
    QApplication app(numberOfArgs, appName.data());

    Q_INIT_RESOURCE(ut_icons);

    SearchLineEdit searchLineEdit(nullptr);

    SECTION(" displays search hint in corresponding label")
    {
        searchLineEdit.setSearchHint("finds actions");

        const auto* child = searchLineEdit.findChild<QLabel*>("searchHint");

        REQUIRE(child != nullptr);
        REQUIRE("finds actions" == child->text().toStdString());
    }

    SECTION(" allows empty search icon")
    {
        searchLineEdit.setSearchIcon(QIcon());

        const auto* child = searchLineEdit.findChild<QLabel*>("searchIcon");

        REQUIRE(child != nullptr);
#if QT_VERSION > QT_VERSION_CHECK(5, 15, 0)
        REQUIRE(0 == child->pixmap(Qt::ReturnByValue).size().width());
        REQUIRE(0 == child->pixmap(Qt::ReturnByValue).size().height());
#else
        REQUIRE(0 == child->pixmap()->size().width());
        REQUIRE(0 == child->pixmap()->size().height());
#endif
    }

    SECTION(" displays a custom search icon")
    {
        searchLineEdit.setSearchIcon(QIcon(":/icons/unit-test/system-search"));

        const auto* child = searchLineEdit.findChild<QLabel*>("searchIcon");

        REQUIRE(child != nullptr);
#if QT_VERSION > QT_VERSION_CHECK(5, 15, 0)
        REQUIRE(16 == child->pixmap(Qt::ReturnByValue).size().width());
        REQUIRE(16 == child->pixmap(Qt::ReturnByValue).size().height());
#else
        REQUIRE(16 == child->pixmap()->size().width());
        REQUIRE(16 == child->pixmap()->size().height());
#endif
    }

    SECTION(" emit the textChanged signal if text in the search box is entered")
    {
        auto* child = searchLineEdit.findChild<QLineEdit*>("searchField");

        QSignalSpy spy(child, SIGNAL(textChanged(const QString&)));

        QTest::keyClicks(child, "hello world");

        REQUIRE(spy.count() > 0);
    }
}