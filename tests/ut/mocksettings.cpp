#include "mocksettings.hpp"

#include <stdexcept>

#include <QVariant>

using aide::test::MockSettings;

void MockSettings::setValue(const HierarchicalId& groupAndKey,
                            const QVariant& value)
{
    const auto key            = groupAndKey.name();
    [[maybe_unused]] auto res = inMemorySettings.emplace(key, value);
    if (!res.second) { inMemorySettings.at(key) = value; }
}

QVariant MockSettings::value(const HierarchicalId& groupAndKey)
{
    return value(groupAndKey, QVariant());
}

QVariant MockSettings::value(const HierarchicalId& groupAndKey,
                             const QVariant& defaultValue)
{
    const auto key = groupAndKey.name();
    if (inMemorySettings.contains(key)) { return inMemorySettings.at(key); }
    return defaultValue;
}

void MockSettings::removeKey(const HierarchicalId& key)
{
    inMemorySettings.erase(key.name());
}

void MockSettings::save() {}
void MockSettings::load() {}
