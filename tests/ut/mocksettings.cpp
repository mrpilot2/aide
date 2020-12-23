#include "mocksettings.hpp"

#include <QVariant>

#include "hierarchicalid.hpp"

using aide::test::MockSettings;

void MockSettings::setValue(const aide::HierarchicalId& group,
                            const std::string& key, const QVariant& value)
{
    auto res = inMemorySettings.emplace(std::make_pair(group, key), value);
    if (not res.second) {
        throw std::runtime_error("Cannot insert into in memory settings");
    }
}

QVariant MockSettings::value(const aide::HierarchicalId& group,
                             const std::string& key)
{
    if (inMemorySettings.find(std::make_pair(group, key)) !=
        inMemorySettings.end()) {
        return inMemorySettings.at(std::make_pair(group, key));
    }
    return QVariant();
}

QVariant MockSettings::value(const aide::HierarchicalId& group,
                             const std::string& key,
                             const QVariant& defaultValue)
{
    if (inMemorySettings.find(std::make_pair(group, key)) !=
        inMemorySettings.end()) {
        return inMemorySettings.at(std::make_pair(group, key));
    }
    return defaultValue;
}

void aide::test::MockSettings::save() {}
void aide::test::MockSettings::load() {}
