#include <catch2/catch_test_macros.hpp>

#include <QLineEdit>
#include <QWidget>

#include <aide/helptopicid.hpp>
#include <aide/widgethelptopic.hpp>

using aide::HelpTopicId;

TEST_CASE("A widget without a Help Topic has an empty one", "[HelpTopic]")
{
    const QWidget widget;

    REQUIRE(aide::helpTopic(&widget).isEmpty());
}

TEST_CASE("A Help Topic can be bound to any widget", "[HelpTopic]")
{
    QLineEdit lineEdit;

    aide::setHelpTopic(&lineEdit, HelpTopicId{"editor"});

    REQUIRE(aide::helpTopic(&lineEdit) == HelpTopicId{"editor"});
}

TEST_CASE("Binding a Help Topic again replaces the previous one", "[HelpTopic]")
{
    QWidget widget;
    aide::setHelpTopic(&widget, HelpTopicId{"first"});

    SECTION("with another topic")
    {
        aide::setHelpTopic(&widget, HelpTopicId{"second"});

        REQUIRE(aide::helpTopic(&widget) == HelpTopicId{"second"});
    }

    SECTION("with an empty topic, which clears it")
    {
        aide::setHelpTopic(&widget, HelpTopicId{});

        REQUIRE(aide::helpTopic(&widget).isEmpty());
    }
}

TEST_CASE("A widget's Help Topic is not inherited by its children",
          "[HelpTopic]")
{
    QWidget parent;
    const QWidget* child = new QWidget(&parent);
    aide::setHelpTopic(&parent, HelpTopicId{"parent"});

    REQUIRE(aide::helpTopic(child).isEmpty());
}

TEST_CASE("A null widget has no Help Topic and binding to it is a no-op",
          "[HelpTopic]")
{
    aide::setHelpTopic(nullptr, HelpTopicId{"ignored"});

    REQUIRE(aide::helpTopic(nullptr).isEmpty());
}
