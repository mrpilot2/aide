#include <memory>
#include <optional>
#include <string>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include <QEventLoop>
#include <QFile>
#include <QProcess>
#include <QStandardPaths>
#include <QTemporaryDir>
#include <QTextStream>

#include "nulllogger.hpp"
#include "vcsprocessrunner.hpp"
#include "vcsrepository.hpp"

using aide::VcsError;
using aide::VcsOperationResult;
using aide::VcsProcessRunner;
using aide::VcsRepository;
using aide::VcsStatusResult;
using aide::test::NullLogger;

namespace
{
    std::optional<std::string> findRealGitExecutable()
    {
        const auto path = QStandardPaths::findExecutable("git");
        if (path.isEmpty()) { return std::nullopt; }
        return path.toStdString();
    }

    // Runs a `git` setup command synchronously (blocking), used only to
    // initialize the temp-dir fixture the test operates on - the operations
    // under test go through VcsRepository/VcsProcessRunner instead.
    void runGitSetupCommand(const std::string& gitExecutable,
                            const QString& workingDirectory,
                            const QStringList& arguments)
    {
        QProcess process;
        process.setWorkingDirectory(workingDirectory);
        process.start(QString::fromStdString(gitExecutable), arguments);
        REQUIRE(process.waitForFinished());
        REQUIRE(process.exitCode() == 0);
    }

    // Returns a resolver that always hands back gitExecutable, without ever
    // moving the value out from under repeat calls.
    VcsRepository::GitExecutableResolver makeResolver(std::string gitExecutable)
    {
        return [gitExecutable = std::move(gitExecutable)] {
            return std::optional<std::string>(gitExecutable);
        };
    }

    VcsOperationResult runStage(VcsRepository& repository,
                                const std::vector<std::string>& paths)
    {
        QEventLoop loop;
        VcsOperationResult result;
        repository.stage(paths, [&](VcsOperationResult opResult) {
            result = std::move(opResult);
            loop.quit();
        });
        loop.exec();
        return result;
    }

    VcsOperationResult runCommit(VcsRepository& repository,
                                 const std::string& message)
    {
        QEventLoop loop;
        VcsOperationResult result;
        repository.commit(message, [&](VcsOperationResult opResult) {
            result = std::move(opResult);
            loop.quit();
        });
        loop.exec();
        return result;
    }

    VcsStatusResult runStatus(VcsRepository& repository)
    {
        QEventLoop loop;
        VcsStatusResult result;
        repository.status([&](VcsStatusResult statusResult) {
            result = std::move(statusResult);
            loop.quit();
        });
        loop.exec();
        return result;
    }

    struct GitRepositorySandbox
    {
        explicit GitRepositorySandbox(const std::string& gitExecutable)
        {
            REQUIRE(dir.isValid());
            runGitSetupCommand(gitExecutable, dir.path(), {"init"});
            runGitSetupCommand(gitExecutable, dir.path(),
                               {"config", "user.email", "test@example.com"});
            runGitSetupCommand(gitExecutable, dir.path(),
                               {"config", "user.name", "Test User"});
        }

        QTemporaryDir dir;
    };
} // namespace

TEST_CASE("VcsRepository stages a file against a real git binary",
          "[VcsRepository][Integration]")
{
    const auto gitExecutable = findRealGitExecutable();
    if (!gitExecutable.has_value()) {
        SUCCEED("Skipping: no git executable found on PATH");
        return;
    }

    const GitRepositorySandbox sandbox(*gitExecutable);

    QFile file(sandbox.dir.filePath("tracked.txt"));
    REQUIRE(file.open(QIODevice::WriteOnly | QIODevice::Text));
    QTextStream(&file) << "hello\n";
    file.close();

    auto runner =
        std::make_shared<VcsProcessRunner>(std::make_shared<NullLogger>());
    VcsRepository repository(sandbox.dir.path().toStdString(), runner,
                             makeResolver(*gitExecutable),
                             std::make_shared<NullLogger>());

    const auto stageResult = runStage(repository, {"tracked.txt"});

    REQUIRE(stageResult.error == VcsError::None);
}

TEST_CASE(
    "VcsRepository staging then committing a file against a real git "
    "binary results in a clean status",
    "[VcsRepository][Integration]")
{
    const auto gitExecutable = findRealGitExecutable();
    if (!gitExecutable.has_value()) {
        SUCCEED("Skipping: no git executable found on PATH");
        return;
    }

    const GitRepositorySandbox sandbox(*gitExecutable);

    QFile file(sandbox.dir.filePath("tracked.txt"));
    REQUIRE(file.open(QIODevice::WriteOnly | QIODevice::Text));
    QTextStream(&file) << "hello\n";
    file.close();

    auto runner =
        std::make_shared<VcsProcessRunner>(std::make_shared<NullLogger>());
    VcsRepository repository(sandbox.dir.path().toStdString(), runner,
                             makeResolver(*gitExecutable),
                             std::make_shared<NullLogger>());

    REQUIRE(runStage(repository, {"tracked.txt"}).error == VcsError::None);
    REQUIRE(runCommit(repository, "Initial commit").error == VcsError::None);

    const auto statusResult = runStatus(repository);

    REQUIRE(statusResult.error == VcsError::None);
    REQUIRE(statusResult.entries.empty());
}

TEST_CASE(
    "VcsRepository operating on a non-repository path surfaces "
    "VcsError::NotARepository against a real git binary",
    "[VcsRepository][Integration]")
{
    const auto gitExecutable = findRealGitExecutable();
    if (!gitExecutable.has_value()) {
        SUCCEED("Skipping: no git executable found on PATH");
        return;
    }

    const QTemporaryDir notARepo;
    REQUIRE(notARepo.isValid());

    auto runner =
        std::make_shared<VcsProcessRunner>(std::make_shared<NullLogger>());
    VcsRepository repository(notARepo.path().toStdString(), runner,
                             makeResolver(*gitExecutable),
                             std::make_shared<NullLogger>());

    const auto statusResult = runStatus(repository);

    REQUIRE(statusResult.error == VcsError::NotARepository);
}
