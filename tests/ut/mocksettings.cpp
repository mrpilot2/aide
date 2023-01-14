#include "mocksettings.hpp"

#include <stdexcept>

#include <QVariant>

using aide::test::MockSettings;

void aide::test::MockSettings::setValue(const aide::HierarchicalId& groupAndKey,
                                        const QVariant& value)
{
    [[maybe_unused]] auto res = inMemorySettings.emplace(groupAndKey, value);
    if (!res.second) { inMemorySettings.at(groupAndKey) = value; }
}

QVariant MockSettings::value(const aide::HierarchicalId& groupAndKey)
{
    return value(groupAndKey, QVariant());
}

QVariant MockSettings::value(const aide::HierarchicalId& groupAndKey,
                             const QVariant& defaultValue)
{
    if (inMemorySettings.find(groupAndKey) != inMemorySettings.end()) {
        return inMemorySettings.at(groupAndKey);
    }
    return defaultValue;
}

void aide::test::MockSettings::removeKey(const aide::HierarchicalId& key)
{
    inMemorySettings.erase(key);
}

void aide::test::MockSettings::save() {}
void aide::test::MockSettings::load() {}
