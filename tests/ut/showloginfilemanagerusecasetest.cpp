
#include <memory>
#include <string>
#include <utility>

#include <catch2/catch_test_macros.hpp>

#include <QCoreApplication>
#include <QFileInfo>
#include <QStandardPaths>
#include <QString>
#include <QTemporaryDir>

#include <aide/logger/loggerfactory.hpp>

#include "mockfilemanagerlauncher.hpp"
#include "recordinglogger.hpp"
#include "showloginfilemanagerusecase.hpp"
#include "standardpathstestguards.hpp"

using aide::core::LoggerFactory;
using aide::core::ShowLogInFileManagerUseCase;
using aide::test::BlockedStandardLocationGuard;
using aide::test::EnvVarGuard;
using aide::test::RecordingLogger;
using aide::test::StandardPathsTestModeGuard;
using aide::tests::MockFileManagerLauncher;

TEST_CASE("A ShowLogInFileManagerUseCase with a resolvable log file path",
          "[ShowLogInFileManagerUseCase]")
{
    const QTemporaryDir sandbox;
    REQUIRE(sandbox.isValid());

    const StandardPathsTestModeGuard testModeGuard;
    const EnvVarGuard homeGuard("HOME", sandbox.path());

    QCoreApplication::setApplicationName(
        "aide_show_log_in_file_manager_use_case_test");

    // Ensures LoggerFactory::logFilePath() has a resolved location to work
    // with, matching the "consistent with a freshly constructed logger"
    // guarantee covered in loggerfactorytest.cpp.
    LoggerFactory::createLogger("show_log_in_file_manager_use_case_test");

    const auto resolvedLogFilePath = LoggerFactory::logFilePath();
    REQUIRE(resolvedLogFilePath.has_value());

    const auto expectedDirectory =
        // NOLINTNEXTLINE(bugprone-unchecked-optional-access)
        QFileInfo(QString::fromStdString(resolvedLogFilePath.value()))
            .absolutePath()
            .toStdString();

    SECTION("asks the launcher to open the log file's containing directory")
    {
        const auto launcher = std::make_shared<MockFileManagerLauncher>();
        const auto logger   = std::make_shared<RecordingLogger>();

        const ShowLogInFileManagerUseCase useCase(launcher, logger);
        useCase.showLogInFileManager();

        REQUIRE(launcher->lastRequestedDirectory.has_value());
        // NOLINTNEXTLINE(bugprone-unchecked-optional-access)
        REQUIRE(launcher->lastRequestedDirectory.value() == expectedDirectory);
        REQUIRE_FALSE(logger->hasWarning());
    }

    SECTION(
        "logs a warning and nothing else when the launcher reports "
        "failure")
    {
        const auto launcher = std::make_shared<MockFileManagerLauncher>();
        launcher->openDirectoryReturnValue = false;
        const auto logger = std::make_shared<RecordingLogger>();

        const ShowLogInFileManagerUseCase useCase(launcher, logger);
        useCase.showLogInFileManager();

        REQUIRE(logger->hasWarning());
    }
}

TEST_CASE("A ShowLogInFileManagerUseCase with an unresolvable log file path",
          "[ShowLogInFileManagerUseCase]")
{
    const QTemporaryDir sandbox;
    REQUIRE(sandbox.isValid());

    const StandardPathsTestModeGuard testModeGuard;

    // CacheLocation is blocked directly rather than via a HOME override -
    // see BlockedStandardLocationGuard for why HOME doesn't reliably
    // isolate it on every platform.
    const BlockedStandardLocationGuard cacheGuard(
        QStandardPaths::CacheLocation);

    // TempLocation reads TMPDIR on Unix and TMP/TEMP on Windows - override
    // all three so the fallback is redirected into our sandbox on every
    // platform, then block it the same way as Cache.
    const EnvVarGuard tmpdirGuard("TMPDIR", sandbox.path() + "/tmp");
    const EnvVarGuard tmpGuard("TMP", sandbox.path() + "/tmp");
    const EnvVarGuard tempGuard("TEMP", sandbox.path() + "/tmp");
    const BlockedStandardLocationGuard tempLocationGuard(
        QStandardPaths::TempLocation);

    REQUIRE_FALSE(LoggerFactory::logFilePath().has_value());

    const auto launcher = std::make_shared<MockFileManagerLauncher>();
    const auto logger   = std::make_shared<RecordingLogger>();

    const ShowLogInFileManagerUseCase useCase(launcher, logger);
    useCase.showLogInFileManager();

    SECTION("does not ask the launcher to open anything")
    {
        REQUIRE_FALSE(launcher->lastRequestedDirectory.has_value());
    }

    SECTION("logs a warning")
    {
        REQUIRE(logger->hasWarning());
    }
}
