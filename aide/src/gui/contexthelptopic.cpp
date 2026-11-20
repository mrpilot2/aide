#include "contexthelptopic.hpp"

#include <QApplication>
#include <QWidget>

#include <aide/widgethelptopic.hpp>

aide::HelpTopicId aide::gui::contextHelpTopic(const QWidget* focusWidget)
{
    for (const auto* widget = focusWidget; widget != nullptr;
         widget = widget->isWindow() ? nullptr : widget->parentWidget()) {
        if (auto topic = helpTopic(widget); !topic.isEmpty()) { return topic; }
    }
    return {};
}

aide::HelpTopicId aide::gui::focusedContextHelpTopic()
{
    return contextHelpTopic(QApplication::focusWidget());
}
