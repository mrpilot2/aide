#include "helptopicid.hpp"

#include <QStringList>

#include "logger/log_helper_macros.hpp"

using aide::HelpTopicId;

namespace
{
    bool isValidHelpTopicId(const QString& id)
    {
        if (id.startsWith('/') || id.contains('?') || id.contains('#')) {
            return false;
        }
        return !id.split('/').contains("..");
    }
} // namespace

HelpTopicId::HelpTopicId(const QString& id)
{
    if (isValidHelpTopicId(id)) {
        m_value = id;
        return;
    }
    AIDE_LOG_WARN(
        "Ignoring invalid help topic id \"{}\": it must be a relative path "
        "without a leading '/', '..' segments, '?' or '#'",
        id.toStdString())
}
