#include "hierarchicalid.hpp"

#include <utility>

using aide::HierarchicalId;

static constexpr char DELIMITER = '/';

HierarchicalId::HierarchicalId(const char* level)
    : m_id{level}
{}

HierarchicalId::HierarchicalId(const const_iterator first,
                               const const_iterator last)
    : m_id{first, last}
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

bool HierarchicalId::operator==(const HierarchicalId& rhs) const
{
    return name() == rhs.name();
}

bool HierarchicalId::operator!=(const HierarchicalId& rhs) const
{
    return !(rhs == *this);
}

bool HierarchicalId::operator<(const HierarchicalId& rhs) const
{
    return name() < rhs.name();
}

bool HierarchicalId::operator>(const HierarchicalId& rhs) const
{
    return rhs < *this;
}

HierarchicalId::const_iterator HierarchicalId::begin() const noexcept
{
    return m_id.begin();
}

HierarchicalId::const_iterator HierarchicalId::end() const noexcept
{
    return m_id.end();
}
