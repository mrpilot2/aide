
#pragma once

#include <cstdint>

namespace aide
{
    namespace utils
    {
        constexpr uint64_t val_64{0xcbf29ce484222325};
        constexpr uint64_t prime_64_const{0x100000001b3};

        constexpr uint64_t hash_64_fnv1a(const char* const str,
                                         uint64_t value = val_64) noexcept
        {
            return (str[0] == '\0')
                       ? value
                       : hash_64_fnv1a(&str[1], (value ^ uint64_t(str[0])) *
                                                    prime_64_const);
        }
    }  // end namespace utils

}  // end namespace aide
