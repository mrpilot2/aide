
#include <catch2/catch_test_macros.hpp>

#include <aide/buildinformation.hpp>

#include "aideinformation.hpp"
#include "aideinformationbuilder.hpp"

using aide::core::AideInformationBuilder;

namespace build_information = aide::build_information;

TEST_CASE("AideInformationBuilder")
{
    const auto info = AideInformationBuilder::buildCurrent();

    SECTION("populates version and git hash from build information")
    {
        REQUIRE(info.versionInfo == build_information::AIDE_VERSION_STRING);
        REQUIRE(info.gitHash == build_information::GIT_HASH);
    }

    SECTION("populates compiler, build type and compile flags")
    {
        REQUIRE(info.compiler == build_information::CMAKE_CXX_COMPILER);
        REQUIRE(info.compilerVersion ==
                build_information::CMAKE_CXX_COMPILER_VERSION);
        REQUIRE(info.buildType == build_information::CMAKE_BUILD_TYPE);
        REQUIRE(info.compileFlags == build_information::COMPILE_FLAGS);
    }

    SECTION("populates a valid build date")
    {
        REQUIRE(info.buildDate.isValid());
    }
}
