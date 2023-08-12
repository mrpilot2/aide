
#ifndef AIDE_UTILS_SYSTEM_MEMORY_HPP
#define AIDE_UTILS_SYSTEM_MEMORY_HPP

#include <cstdint>
#include <optional>

namespace aide::utils
{
    class SystemMemory
    {
    public:
        static std::optional<uint64_t> getAvailableRAMInBytes();
    };
} // namespace aide::utils

#endif // AIDE_UTILS_SYSTEM_MEMORY_HPP
