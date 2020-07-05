#include "hierarchicalid.hpp"

#include <utility>

using aide::HierarchicalId;

static const char DELIMITER = '/';

HierarchicalId::HierarchicalId(const char* level)
    : m_id{level}
{}

HierarchicalId::HierarchicalId(std::vector<const char*> ids)
    : m_id{std::move(ids)}
{}

HierarchicalId& HierarchicalId::operator()(const char* level)
{
    return addLevel(level);
}

HierarchicalId HierarchicalId::operator()(const char* level) const
{
    return addLevel(level);
}

HierarchicalId& HierarchicalId::addLevel(const char* level)
{
    m_id.emplace_back(level);
    return *this;
}

HierarchicalId HierarchicalId::addLevel(const char* level) const
{
    std::vector<const char*> newId{m_id};
    newId.emplace_back(level);
    return HierarchicalId(newId);
}

std::string HierarchicalId::name() const
{
    std::string result;

    for (const auto& c : m_id) {
        result += c;
        result += DELIMITER;
    }
    return result.erase(result.size() - 1);
}
