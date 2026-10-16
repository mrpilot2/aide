#include "widgethelptopic.hpp"

#include <QVariant>
#include <QWidget>

namespace
{
    // Stored as a dynamic property, so any widget can carry a topic without
    // aIDE subclassing it.
    constexpr const char* HELP_TOPIC_PROPERTY{"aideHelpTopic"};
} // namespace

void aide::setHelpTopic(QWidget* widget, const HelpTopicId& id)
{
    if (widget == nullptr) { return; }

    // An invalid QVariant removes the dynamic property again.
    widget->setProperty(HELP_TOPIC_PROPERTY,
                        id.isEmpty() ? QVariant{} : QVariant::fromValue(id));
}

aide::HelpTopicId aide::helpTopic(const QWidget* widget)
{
    if (widget == nullptr) { return {}; }

    return widget->property(HELP_TOPIC_PROPERTY).value<HelpTopicId>();
}
