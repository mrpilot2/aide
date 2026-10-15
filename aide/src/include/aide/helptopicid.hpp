#ifndef AIDE_HELP_TOPIC_ID_HPP
#define AIDE_HELP_TOPIC_ID_HPP

#include <QMetaType>
#include <QString>

namespace aide
{
    /**
     * @brief Identifies a Help Topic: a relative path that, unless the client
     * remaps it, is the topic's location under the Help Base URL (e.g.
     * `keymap`, `settings/keymap`).
     *
     * Nesting with `/` is allowed. A leading `/`, a `..` segment, `?` or `#`
     * make the input invalid: it then becomes an empty ID (no topic) and a
     * warning is logged. No exceptions are thrown.
     */
    class HelpTopicId
    {
    public:
        HelpTopicId() = default;

        explicit HelpTopicId(const QString& id);

        [[nodiscard]] const QString& value() const { return m_value; }

        [[nodiscard]] bool isEmpty() const { return m_value.isEmpty(); }

        bool operator==(const HelpTopicId& rhs) const
        {
            return m_value == rhs.m_value;
        }

        bool operator!=(const HelpTopicId& rhs) const
        {
            return !(*this == rhs);
        }

        bool operator<(const HelpTopicId& rhs) const
        {
            return m_value < rhs.m_value;
        }

    private:
        QString m_value;
    };
} // namespace aide

Q_DECLARE_METATYPE(aide::HelpTopicId)

#endif // AIDE_HELP_TOPIC_ID_HPP
