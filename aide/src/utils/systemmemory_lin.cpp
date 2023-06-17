#include <sys/sysinfo.h>

#include "systemmemory.hpp"

using aide::utils::SystemMemory;

std::optional<uint64_t> SystemMemory::getAvailableRAMInBytes()
{
    struct sysinfo info
    {
    };
    if (sysinfo(&info) == -1) { return std::nullopt; }
    return info.totalram;
}