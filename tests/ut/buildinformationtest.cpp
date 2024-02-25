#include <string>

#include <catch2/catch_test_macros.hpp>

#include <aide/buildinformation.hpp>

using aide::build_information::GIT_HASH;

TEST_CASE("Test version number check")
{
    static_assert(AIDE_VERSION >= AIDE_VERSION_CHECK(0, 0, 0),
                  "Version should always be greater than 0.0.0");

    REQUIRE(AIDE_VERSION >= AIDE_VERSION_CHECK(0, 0, 0));
}

TEST_CASE("Git Hash")
{
    SECTION("shall not be empty")
    {
        const std::string hash{GIT_HASH};
        REQUIRE(!hash.empty());
    }

    SECTION("shall not contain NOTFOUND")
    {
        const std::string hash{GIT_HASH};
        const size_t res{hash.find("NOTFOUND")};
        REQUIRE(res == std::string::npos);
    }
}