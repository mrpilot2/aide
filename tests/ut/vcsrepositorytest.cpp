#include <optional>
#include <string>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "fakevcsprocessrunner.hpp"
#include "nulllogger.hpp"
#include "vcsrepository.hpp"

using aide::VcsError;
using aide::VcsFileState;
using aide::VcsFileStatus;
using aide::VcsOperationResult;
using aide::VcsRepository;
using aide::VcsStatusResult;
using aide::test::FakeVcsProcessRunner;
using aide::test::NullLogger;

namespace
{
    constexpr auto kRootPath              = "/repo/root";
    constexpr auto kGitExecutable         = "git";
    constexpr int kNotARepositoryExitCode = 128;

    std::shared_ptr<VcsRepository> makeRepository(
        const std::shared_ptr<FakeVcsProcessRunner>& runner,
        std::optional<std::string> gitExecutable = kGitExecutable)
    {
        return std::make_shared<VcsRepository>(
            kRootPath, runner,
            [gitExecutable = std::move(gitExecutable)] {
                return gitExecutable;
            },
            std::make_shared<NullLogger>());
    }

    // Runs a single `git status --porcelain=v1` round-trip through a fake
    // process runner returning the given canned stdout, and returns the
    // parsed entries. Asserting the round-trip itself succeeded here (once)
    // keeps each caller's own assertions - and cognitive complexity - small.
    std::vector<VcsFileStatus> parseStatusOutput(const std::string& output)
    {
        auto runner                 = std::make_shared<FakeVcsProcessRunner>();
        auto repository             = makeRepository(runner);
        runner->nextResult.exitCode = 0;
        runner->nextResult.standardOutput = output;

        std::optional<VcsStatusResult> result;
        repository->status([&result](VcsStatusResult statusResult) {
            result = std::move(statusResult);
        });

        REQUIRE(result.has_value());
        // NOLINTBEGIN(bugprone-unchecked-optional-access)
        REQUIRE(result->error == VcsError::None);
        return result->entries;
        // NOLINTEND(bugprone-unchecked-optional-access)
    }
} // namespace

TEST_CASE("VcsRepository::status reports one entry per output line",
          "[VcsRepository]")
{
    const auto entries = parseStatusOutput(
        "M  staged_modified.txt\n"
        " M unstaged_modified.txt\n"
        "A  new_file.txt\n"
        "?? untracked.txt\n"
        "R  old_name.txt -> new_name.txt\n");

    REQUIRE(entries.size() == 5);
}

TEST_CASE("VcsRepository::status reports a staged modification",
          "[VcsRepository]")
{
    const auto entries = parseStatusOutput("M  staged_modified.txt\n");

    REQUIRE(entries.size() == 1);
    REQUIRE(entries[0].path == "staged_modified.txt");
    REQUIRE(entries[0].indexState == VcsFileState::Modified);
    REQUIRE(entries[0].workTreeState == VcsFileState::Unmodified);
}

TEST_CASE("VcsRepository::status reports an unstaged modification",
          "[VcsRepository]")
{
    const auto entries = parseStatusOutput(" M unstaged_modified.txt\n");

    REQUIRE(entries.size() == 1);
    REQUIRE(entries[0].path == "unstaged_modified.txt");
    REQUIRE(entries[0].indexState == VcsFileState::Unmodified);
    REQUIRE(entries[0].workTreeState == VcsFileState::Modified);
}

TEST_CASE("VcsRepository::status reports a staged addition", "[VcsRepository]")
{
    const auto entries = parseStatusOutput("A  new_file.txt\n");

    REQUIRE(entries.size() == 1);
    REQUIRE(entries[0].path == "new_file.txt");
    REQUIRE(entries[0].indexState == VcsFileState::Added);
}

TEST_CASE("VcsRepository::status reports an untracked file on both columns",
          "[VcsRepository]")
{
    const auto entries = parseStatusOutput("?? untracked.txt\n");

    REQUIRE(entries.size() == 1);
    REQUIRE(entries[0].path == "untracked.txt");
    REQUIRE(entries[0].indexState == VcsFileState::Untracked);
    REQUIRE(entries[0].workTreeState == VcsFileState::Untracked);
}

TEST_CASE("VcsRepository::status reports a rename using its new path",
          "[VcsRepository]")
{
    const auto entries = parseStatusOutput("R  old_name.txt -> new_name.txt\n");

    REQUIRE(entries.size() == 1);
    REQUIRE(entries[0].path == "new_name.txt");
    REQUIRE(entries[0].indexState == VcsFileState::Renamed);
}

TEST_CASE(
    "VcsRepository::status issues `status --porcelain=v1` against "
    "the repository root",
    "[VcsRepository]")
{
    auto runner     = std::make_shared<FakeVcsProcessRunner>();
    auto repository = makeRepository(runner);

    runner->nextResult.exitCode = 0;

    repository->status([](const VcsStatusResult&) {});

    REQUIRE(runner->callCount == 1);
    REQUIRE(runner->lastWorkingDirectory == kRootPath);
    REQUIRE(runner->lastExecutable == kGitExecutable);
    REQUIRE(runner->lastArguments ==
            std::vector<std::string>{"status", "--porcelain=v1"});
}

TEST_CASE(
    "VcsRepository::status on a non-repository path surfaces "
    "VcsError::NotARepository",
    "[VcsRepository]")
{
    auto runner     = std::make_shared<FakeVcsProcessRunner>();
    auto repository = makeRepository(runner);

    runner->nextResult.exitCode = kNotARepositoryExitCode;
    runner->nextResult.standardError =
        "fatal: not a git repository (or any of the parent "
        "directories): .git\n";

    std::optional<VcsStatusResult> result;
    repository->status([&result](VcsStatusResult statusResult) {
        result = std::move(statusResult);
    });

    REQUIRE(result.has_value());
    // NOLINTBEGIN(bugprone-unchecked-optional-access)
    REQUIRE(result->error == VcsError::NotARepository);
    REQUIRE(result->entries.empty());
    // NOLINTEND(bugprone-unchecked-optional-access)
}

TEST_CASE(
    "VcsRepository::status with no resolvable git executable surfaces "
    "VcsError::GitNotFound without spawning a process",
    "[VcsRepository]")
{
    auto runner     = std::make_shared<FakeVcsProcessRunner>();
    auto repository = makeRepository(runner, std::nullopt);

    std::optional<VcsStatusResult> result;
    repository->status([&result](VcsStatusResult statusResult) {
        result = std::move(statusResult);
    });

    REQUIRE(result.has_value());
    // NOLINTBEGIN(bugprone-unchecked-optional-access)
    REQUIRE(result->error == VcsError::GitNotFound);
    // NOLINTEND(bugprone-unchecked-optional-access)
    REQUIRE(runner->callCount == 0);
}

TEST_CASE("VcsRepository::stage runs `git add --` with the given paths",
          "[VcsRepository]")
{
    auto runner     = std::make_shared<FakeVcsProcessRunner>();
    auto repository = makeRepository(runner);

    runner->nextResult.exitCode = 0;

    std::optional<VcsOperationResult> result;
    repository->stage({"a.txt", "b.txt"},
                      [&result](VcsOperationResult operationResult) {
                          result = std::move(operationResult);
                      });

    REQUIRE(result.has_value());
    // NOLINTBEGIN(bugprone-unchecked-optional-access)
    REQUIRE(result->error == VcsError::None);
    // NOLINTEND(bugprone-unchecked-optional-access)
    REQUIRE(runner->lastArguments ==
            std::vector<std::string>{"add", "--", "a.txt", "b.txt"});
}

TEST_CASE(
    "VcsRepository::stage with no resolvable git executable surfaces "
    "VcsError::GitNotFound without spawning a process",
    "[VcsRepository]")
{
    auto runner     = std::make_shared<FakeVcsProcessRunner>();
    auto repository = makeRepository(runner, std::nullopt);

    std::optional<VcsOperationResult> result;
    repository->stage({"a.txt"}, [&result](VcsOperationResult operationResult) {
        result = std::move(operationResult);
    });

    REQUIRE(result.has_value());
    // NOLINTBEGIN(bugprone-unchecked-optional-access)
    REQUIRE(result->error == VcsError::GitNotFound);
    // NOLINTEND(bugprone-unchecked-optional-access)
    REQUIRE(runner->callCount == 0);
}

TEST_CASE("VcsRepository::unstage runs `git reset --` with the given paths",
          "[VcsRepository]")
{
    auto runner     = std::make_shared<FakeVcsProcessRunner>();
    auto repository = makeRepository(runner);

    runner->nextResult.exitCode = 0;

    std::optional<VcsOperationResult> result;
    repository->unstage({"a.txt"},
                        [&result](VcsOperationResult operationResult) {
                            result = std::move(operationResult);
                        });

    REQUIRE(result.has_value());
    // NOLINTBEGIN(bugprone-unchecked-optional-access)
    REQUIRE(result->error == VcsError::None);
    // NOLINTEND(bugprone-unchecked-optional-access)
    REQUIRE(runner->lastArguments ==
            std::vector<std::string>{"reset", "--", "a.txt"});
}

TEST_CASE("VcsRepository::commit succeeds and runs `git commit -m <message>`",
          "[VcsRepository]")
{
    auto runner     = std::make_shared<FakeVcsProcessRunner>();
    auto repository = makeRepository(runner);

    runner->nextResult.exitCode = 0;

    std::optional<VcsOperationResult> result;
    repository->commit("A commit message",
                       [&result](VcsOperationResult operationResult) {
                           result = std::move(operationResult);
                       });

    REQUIRE(result.has_value());
    // NOLINTBEGIN(bugprone-unchecked-optional-access)
    REQUIRE(result->error == VcsError::None);
    // NOLINTEND(bugprone-unchecked-optional-access)
    REQUIRE(runner->lastArguments ==
            std::vector<std::string>{"commit", "-m", "A commit message"});
}

TEST_CASE(
    "VcsRepository::commit surfaces a git failure and its detail "
    "message",
    "[VcsRepository]")
{
    auto runner     = std::make_shared<FakeVcsProcessRunner>();
    auto repository = makeRepository(runner);

    runner->nextResult.exitCode      = 1;
    runner->nextResult.standardError = "nothing to commit\n";

    std::optional<VcsOperationResult> result;
    repository->commit("A commit message",
                       [&result](VcsOperationResult operationResult) {
                           result = std::move(operationResult);
                       });

    REQUIRE(result.has_value());
    // NOLINTBEGIN(bugprone-unchecked-optional-access)
    REQUIRE(result->error == VcsError::Other);
    REQUIRE(result->details == "nothing to commit\n");
    // NOLINTEND(bugprone-unchecked-optional-access)
}

TEST_CASE("VcsRepository::rootPath returns the constructed root",
          "[VcsRepository]")
{
    auto runner     = std::make_shared<FakeVcsProcessRunner>();
    auto repository = makeRepository(runner);

    REQUIRE(repository->rootPath() == kRootPath);
}
