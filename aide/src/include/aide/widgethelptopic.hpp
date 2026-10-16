#ifndef AIDE_WIDGET_HELP_TOPIC_HPP
#define AIDE_WIDGET_HELP_TOPIC_HPP

#include <aide/helptopicid.hpp>

class QWidget;

namespace aide
{
    /**
     * @brief Binds a Help Topic to a widget, for Context Help.
     *
     * Works on any widget, including stock Qt ones. Calling it again
     * replaces the topic; an empty ID clears it. A null widget is ignored.
     *
     * Context Help resolves the focused widget's topic by walking up the
     * parent chain to the first widget with a topic, stopping at the focused
     * widget's window, so a topic on a container covers its children.
     */
    void setHelpTopic(QWidget* widget, const HelpTopicId& id);

    /**
     * @brief The Help Topic bound to this widget itself, or an empty ID.
     *
     * Parents are not consulted.
     */
    [[nodiscard]] HelpTopicId helpTopic(const QWidget* widget);
} // namespace aide

#endif // AIDE_WIDGET_HELP_TOPIC_HPP
