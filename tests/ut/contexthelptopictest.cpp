#include <catch2/catch_test_macros.hpp>

#include <QLineEdit>
#include <QWidget>

#include <aide/helptopicid.hpp>
#include <aide/widgethelptopic.hpp>

#include "contexthelptopic.hpp"

using aide::HelpTopicId;
using aide::gui::contextHelpTopic;

TEST_CASE("Context Help resolves the nearest widget's topic",
          "[ContextHelpTopic]")
{
    QWidget container;
    auto* lineEdit = new QLineEdit(&container);
    aide::setHelpTopic(&container, HelpTopicId{"container"});

    SECTION("so a container's topic covers its focused child")
    {
        REQUIRE(contextHelpTopic(lineEdit) == HelpTopicId{"container"});
    }

    SECTION("so a topic on the focused child wins over the container's")
    {
        aide::setHelpTopic(lineEdit, HelpTopicId{"line-edit"});

        REQUIRE(contextHelpTopic(lineEdit) == HelpTopicId{"line-edit"});
    }
}

TEST_CASE("Context Help never walks past the focused widget's window",
          "[ContextHelpTopic]")
{
    QWidget parentWindow;
    aide::setHelpTopic(&parentWindow, HelpTopicId{"parent-window"});
    auto* childWindow = new QWidget(&parentWindow, Qt::Window);
    auto* lineEdit    = new QLineEdit(childWindow);

    SECTION("so a parent window's topic is ignored")
    {
        REQUIRE(contextHelpTopic(lineEdit).isEmpty());
    }

    SECTION("but the window's own topic is used")
    {
        aide::setHelpTopic(childWindow, HelpTopicId{"child-window"});

        REQUIRE(contextHelpTopic(lineEdit) == HelpTopicId{"child-window"});
    }
}

TEST_CASE("Context Help resolves no topic", "[ContextHelpTopic]")
{
    SECTION("when no widget up to the window has one")
    {
        QWidget window;
        const auto* lineEdit = new QLineEdit(&window);

        REQUIRE(contextHelpTopic(lineEdit).isEmpty());
    }

    SECTION("when nothing has focus")
    {
        REQUIRE(contextHelpTopic(nullptr).isEmpty());
    }
}
