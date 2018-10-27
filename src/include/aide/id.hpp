
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
            , m_name{description}
        {
        }

        /**
         * The hashed description represented as HashValue
         * @return Returns the hashed value
         */
        constexpr HashValue value() const noexcept
        {
            return m_id;
        }

        /**
         * The name given in the constructor of the UID
         * @return the name
         */
        const char* name() const;

        /**
         * Compares two IDs and returns true if the own ID is alphabetically
         * below the other one
         * @param other The other ID
         * @return True if the own ID is alphabetically lower
         */
        bool isAlphabeticallyBefore(const Id& other) const;

    private:
        const HashValue m_id{0};

        const char* const m_name;
    };

}  // end namespace aide
