
#include <sys/sysctl.h>

#include "systemmemory.hpp"

using aide::utils::SystemMemory;

std::optional<uint64_t> SystemMemory::getAvailableRAMInBytes()
{
    int mib[]     = {CTL_HW, HW_MEMSIZE};
    int64_t value = 0;
    size_t length = sizeof(value);

    if (-1 == sysctl(mib, 2, &value, &length, NULL, 0)) {
        return std::nullopt;
    } else {
        return value;
    }
}
