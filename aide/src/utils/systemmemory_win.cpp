
#include <windows.h>

#include "aide/utils/systemmemory.hpp"

using aide::utils::SystemMemory;

std::optional<uint64_t> SystemMemory::getAvailableRAMInBytes()
{
    MEMORYSTATUSEX memory_status;
    ZeroMemory(&memory_status, sizeof(MEMORYSTATUSEX));
    memory_status.dwLength = sizeof(MEMORYSTATUSEX);
    if (GlobalMemoryStatusEx(&memory_status) != 0) {
        return memory_status.ullTotalPhys;
    }

    return std::nullopt;
}
