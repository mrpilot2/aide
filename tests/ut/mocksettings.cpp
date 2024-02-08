#include "mocksettings.hpp"

#include <stdexcept>

#include <QVariant>

using aide::test::MockSettings;

void MockSettings::setValue(const HierarchicalId& groupAndKey,
                            const QVariant& value)
{
    [[maybe_unused]] auto res = inMemorySettings.emplace(groupAndKey, value);
    if (!res.second) { inMemorySettings.at(groupAndKey) = value; }
}

QVariant MockSettings::value(const HierarchicalId& groupAndKey)
{
    return value(groupAndKey, QVariant());
}

QVariant MockSettings::value(const HierarchicalId& groupAndKey,
                             const QVariant& defaultValue)
{
    if (inMemorySettings.find(groupAndKey) != inMemorySettings.end()) {
        return inMemorySettings.at(groupAndKey);
    }
    return defaultValue;
}

void MockSettings::removeKey(const HierarchicalId& key)
{
    inMemorySettings.erase(key);
}

void MockSettings::save() {}
void MockSettings::load() {}
