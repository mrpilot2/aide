
#include <catch2/catch.hpp>

#include <array>

#include <aide/application.hpp>

TEST_CASE("Test application construction")
{
    // NOLINTNEXTLINE
    std::array<char*, 1> appName{{const_cast<char*>("aide")}};

    aide::Application app(1, appName.data());
}
