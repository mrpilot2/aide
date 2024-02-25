#include <catch2/catch_test_macros.hpp>

#include "aide/utils/systemmemory.hpp"

using aide::utils::SystemMemory;

TEST_CASE("Reasonable memory information")
{
    REQUIRE(SystemMemory::getAvailableRAMInBytes() > 1024 * 1024 * 1000);
}