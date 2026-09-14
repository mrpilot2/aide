#include <string>

#include <catch2/catch_test_macros.hpp>

#include "vcscommandlineredactor.hpp"

using aide::redactVcsCommandLineCredentials;

TEST_CASE("Redacting credentials embedded in a git command line's URL",
          "[VcsCommandLineRedactor]")
{
    SECTION("masks a user:password@ credential behind a scheme")
    {
        REQUIRE(redactVcsCommandLineCredentials(
                    "git push https://alice:s3cr3t@example.com/repo.git") ==
                "git push https://alice:***@example.com/repo.git");
    }

    SECTION("masks a user:token@ credential without a scheme")
    {
        REQUIRE(redactVcsCommandLineCredentials(
                    "git fetch alice:ghp_abc123@example.com/repo.git") ==
                "git fetch alice:***@example.com/repo.git");
    }

    SECTION("leaves a command line with no embedded credentials unchanged")
    {
        REQUIRE(redactVcsCommandLineCredentials("git status --porcelain=v1") ==
                "git status --porcelain=v1");
    }

    SECTION("leaves a plain https URL with no credentials unchanged")
    {
        REQUIRE(redactVcsCommandLineCredentials(
                    "git push https://example.com/repo.git") ==
                "git push https://example.com/repo.git");
    }
}
