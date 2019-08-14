
#include "id.hpp"

#include <algorithm>
#include <cstring>

using aide::Id;

const std::string& Id::name() const
{
    return m_name;
}

bool Id::isAlphabeticallyBefore(const Id& other) const
{
    return std::lexicographical_compare(m_name.begin(), m_name.end(),
                                        other.name().begin(),
                                        other.name().end());
}
