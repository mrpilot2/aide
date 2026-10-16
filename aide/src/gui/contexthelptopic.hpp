#ifndef AIDE_CONTEXT_HELP_TOPIC_HPP
#define AIDE_CONTEXT_HELP_TOPIC_HPP

#include <aide/helptopicid.hpp>

class QWidget;

namespace aide::gui
{
    // The Help Topic Context Help opens for @p focusWidget: the topic of the
    // first widget with one, walking from @p focusWidget up its parents. The
    // walk stops at the focused widget's window, whose own topic still
    // counts, so a topic on a dialog covers everything inside it but a parent
    // window's topic is ignored. Empty if none is found or @p focusWidget is
    // null.
    [[nodiscard]] HelpTopicId contextHelpTopic(const QWidget* focusWidget);
} // namespace aide::gui

#endif // AIDE_CONTEXT_HELP_TOPIC_HPP
