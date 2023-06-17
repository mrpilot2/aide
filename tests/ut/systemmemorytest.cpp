#include <catch2/catch.hpp>

#include "systemmemory.hpp"

using aide::utils::SystemMemory;

TEST_CASE("Reasonable memory information")
{
    REQUIRE(SystemMemory::getAvailableRAMInBytes() > 1024 * 1024 * 1000);
}