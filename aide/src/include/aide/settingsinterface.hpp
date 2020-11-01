#ifndef AIDE_SETTINGS_INTERFACE_HPP
#define AIDE_SETTINGS_INTERFACE_HPP

#include <string>

#include <QVariant>

namespace aide
{
    class HierarchicalId;

    class SettingsInterface
    {
    public:
        virtual ~SettingsInterface();

        virtual void setValue(const HierarchicalId& group, std::string key,
                              const QVariant& value) = 0;

        virtual QVariant value(const HierarchicalId& group)    = 0;
        virtual QVariant value(const HierarchicalId& group,
                               const QVariant& defaultValue()) = 0;

        virtual void save() = 0;
        virtual void load() = 0;
    };
} // namespace aide

#endif // AIDE_SETTINGS_INTERFACE_HPP
