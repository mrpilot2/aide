
#pragma once

#include <cstdint>

#include "hash.hpp"

namespace aide
{
    /**
     * The representation of a hashed value
     */
    using HashValue = uint64_t;

    /**
     * @brief Generates a UID from a description
     *
     * The class ID generates a UID for a given description. This UID is unique
     * within the process and is used to identify for example Actions.
     */
    class Id
    {
    public:
        Id() = delete;

        /**
         * Constructs a UID for a given description
         * @param description The description of the constructed ID
         */
        constexpr explicit Id(char const* const description) noexcept
            : m_id{aide::utils::hash_64_fnv1a(description)}
        {
        }

        /**
         * The hashed description represented as HashValue
         * @return Returns the hashed value
         */
        constexpr HashValue value() const
        {
            return m_id;
        }

    private:
        HashValue m_id;
    };

}  // end namespace aide
