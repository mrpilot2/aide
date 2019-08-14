
#pragma once

#include <cstdint>
#include <string>
#include <type_traits>

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
         * Constructs a UID for a given description or multiple descriptions
         *
         * Multiple descriptions constructs kind of a hierarchy for grouping
         *
         * @param description The description of the constructed ID
         */
        template <typename First, typename... Args,
                  typename = std::enable_if_t<
                      std::is_convertible_v<First, std::string>, std::string>>
        explicit Id(First description, Args... args)
        {
            append(description, std::forward<Args>(args)...);
            m_name = m_name.substr(0, m_name.size() - 2);
            m_id = aide::utils::hash_64_fnv1a(m_name.c_str());
        }

        /**
         * The hashed description represented as HashValue
         * @return Returns the hashed value
         */
        [[nodiscard]] constexpr HashValue value() const noexcept
        {
            return m_id;
        }

        /**
         * The name given in the constructor of the UID
         * @return the name
         */
        [[nodiscard]] const std::string& name() const;

        /**
         * Compares two IDs and returns true if the own ID is alphabetically
         * below the other one
         * @param other The other ID
         * @return True if the own ID is alphabetically lower
         */
        [[nodiscard]] bool isAlphabeticallyBefore(const Id& other) const;

    private:
        template <typename T, typename... Args>
        void append(T description, Args... args)
        {
            append(description);
            append(args...);
        }

        template <typename T>
        void append(T description)
        {
            m_name.append(description);
            m_name.append("::");
        }

        HashValue m_id{0};

        std::string m_name;
    };

} // end namespace aide
