
#include "id.hpp"

#include <algorithm>
#include <cstring>

using aide::Id;

const char* Id::name() const
{
    return m_name;
}

bool Id::isAlphabeticallyBefore(const Id& other) const

{
    return std::lexicographical_compare(m_name, m_name + strlen(m_name),
                                        other.name(),
                                        other.name() + strlen(other.name()));
}
