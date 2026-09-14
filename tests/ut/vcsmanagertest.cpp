#include <string>

#include <catch2/catch_test_macros.hpp>

#include "fakevcsprocessrunner.hpp"
#include "nulllogger.hpp"
#include "vcsmanager.hpp"

using aide::VcsManager;
using aide::test::FakeVcsProcessRunner;
using aide::test::NullLogger;

TEST_CASE("VcsManager repository registration", "[VcsManager]")
{
    VcsManager manager(std::make_shared<FakeVcsProcessRunner>(),
                       std::make_shared<NullLogger>());

    SECTION("has no registered repositories initially")
    {
        REQUIRE(manager.registeredRepositories().empty());
    }

    SECTION("addRepository registers the path and returns a handle for it")
    {
        auto repository = manager.addRepository("/repo/one");

        REQUIRE(repository != nullptr);
        REQUIRE(repository->rootPath() == "/repo/one");
        REQUIRE(manager.registeredRepositories() ==
                std::vector<std::string>{"/repo/one"});
    }

    SECTION("addRepository can register multiple roots")
    {
        manager.addRepository("/repo/one");
        manager.addRepository("/repo/two");

        REQUIRE(manager.registeredRepositories().size() == 2);
    }
}

TEST_CASE("VcsManager git executable resolution", "[VcsManager]")
{
    VcsManager manager(std::make_shared<FakeVcsProcessRunner>(),
                       std::make_shared<NullLogger>());

    SECTION("setGitExecutablePath overrides the resolved path")
    {
        manager.setGitExecutablePath("/custom/git");

        REQUIRE(manager.gitExecutablePath() == "/custom/git");
    }

    SECTION("resolving the git executable is idempotent")
    {
        REQUIRE(manager.gitExecutablePath() == manager.gitExecutablePath());
    }
}
