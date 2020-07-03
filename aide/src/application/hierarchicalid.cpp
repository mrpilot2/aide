#include "hierarchicalid.hpp"

using aide::HierarchicalId;

HierarchicalId::HierarchicalId(const char* level)
    : m_id{level}
{}

HierarchicalId& aide::HierarchicalId::operator()(const char* level)
{
    return addLevel(level);
}

HierarchicalId& HierarchicalId::addLevel(const char* level)
{
    m_id.push_back(level);
    return *this;
}

std::string HierarchicalId::name() const
{
    std::string result;

    for (const auto& c : m_id) {
        result += c;
        result += "/";
    }
    return result.erase(result.size() - 1);
}
