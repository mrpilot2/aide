#include <cstdio>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <utility>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include <QCoreApplication>
#include <QStandardPaths>
#include <QString>
#include <QTemporaryDir>

#include <aide/logger/loggerfactory.hpp>

#include "standardpathstestguards.hpp"

using aide::core::LoggerFactory;
using aide::test::BlockedStandardLocationGuard;
using aide::test::EnvVarGuard;
using aide::test::StandardPathsTestModeGuard;

namespace
{
    std::string getFileContents(const std::string& fileName)
    {
        const std::ifstream logFile(fileName, std::ios::in);
        std::stringstream fileContent;
        fileContent << logFile.rdbuf();
        return fileContent.str();
    }
} // namespace

TEST_CASE(
    "LoggerFactory::logFilePath is consistent with a freshly constructed "
    "logger",
    "[LoggerFactory]")
{
    const QTemporaryDir sandbox;
    REQUIRE(sandbox.isValid());

    const StandardPathsTestModeGuard testModeGuard;
    const EnvVarGuard homeGuard("HOME", sandbox.path());

    QCoreApplication::setApplicationName(
        "aide_logger_factory_consistency_test");

    auto logger =
        LoggerFactory::createLogger("logger_factory_consistency_test");

    const auto resolvedPath = LoggerFactory::logFilePath();
    REQUIRE(resolvedPath.has_value());

    logger->info("logFilePath consistency probe");
    logger->flush();

    // NOLINTNEXTLINE(bugprone-unchecked-optional-access)
    REQUIRE_THAT(
        ::getFileContents(resolvedPath.value()),
        Catch::Matchers::ContainsSubstring("logFilePath consistency probe"));
}

TEST_CASE(
    "LoggerFactory::logFilePath is absent when no writable location "
    "exists",
    "[LoggerFactory]")
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
}

TEST_CASE(
    "LoggerFactory::createLogger still returns a working logger when no "
    "writable location exists",
    "[LoggerFactory]")
{
    const QTemporaryDir sandbox;
    REQUIRE(sandbox.isValid());

    const StandardPathsTestModeGuard testModeGuard;

    // See the "logFilePath is absent" test above for why CacheLocation is
    // blocked directly rather than via a HOME override.
    const BlockedStandardLocationGuard cacheGuard(
        QStandardPaths::CacheLocation);

    const EnvVarGuard tmpdirGuard("TMPDIR", sandbox.path() + "/tmp");
    const EnvVarGuard tmpGuard("TMP", sandbox.path() + "/tmp");
    const EnvVarGuard tempGuard("TEMP", sandbox.path() + "/tmp");
    const BlockedStandardLocationGuard tempLocationGuard(
        QStandardPaths::TempLocation);

    // The fallback logger hardcodes FileName("aide.log"), which the
    // rotating sink writes relative to the current working directory - see
    // "Logger constructor variants" in loggertest.cpp for the same
    // technique applied to Logger's default constructor directly.
    const std::filesystem::path previousWorkingDir{
        std::filesystem::current_path()};
    std::filesystem::current_path(TEST_LOG_FILE_LOCATION);

    const auto logger =
        LoggerFactory::createLogger("logger_factory_fallback_test");

    REQUIRE(logger != nullptr);
    REQUIRE_NOTHROW(logger->info("fallback logger probe"));
    REQUIRE_NOTHROW(logger->flush());

    std::filesystem::current_path(previousWorkingDir);
    [[maybe_unused]] auto res =
        std::remove((std::filesystem::path{TEST_LOG_FILE_LOCATION} / "aide.log")
                        .string()
                        .c_str());
}
