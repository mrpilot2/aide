#include <fstream>
#include <sstream>
#include <string>
#include <utility>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include <QCoreApplication>
#include <QFile>
#include <QFileDevice>
#include <QStandardPaths>
#include <QString>
#include <QTemporaryDir>

#include <aide/logger/loggerfactory.hpp>

#include "standardpathstestguards.hpp"

using aide::core::LoggerFactory;
using aide::test::EnvVarGuard;
using aide::test::StandardPathsTestModeGuard;
using aide::test::UnwritableCacheLocationGuard;

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

    // Strip write permission from the sandbox root so QDir::mkpath() can
    // never create a subdirectory underneath it - this simulates "no
    // writable location" for the temp-dir fallback.
    REQUIRE(QFile::setPermissions(
        sandbox.path(), QFileDevice::ReadOwner | QFileDevice::ExeOwner));

    const StandardPathsTestModeGuard testModeGuard;

    // CacheLocation is blocked directly rather than via a HOME override -
    // see UnwritableCacheLocationGuard for why HOME doesn't reliably
    // isolate it on every platform.
    const UnwritableCacheLocationGuard cacheGuard;

    // TempLocation reads TMPDIR on Unix and TMP/TEMP on Windows - override
    // all three so the fallback is blocked on every platform.
    const EnvVarGuard tmpdirGuard("TMPDIR", sandbox.path() + "/tmp");
    const EnvVarGuard tmpGuard("TMP", sandbox.path() + "/tmp");
    const EnvVarGuard tempGuard("TEMP", sandbox.path() + "/tmp");

    REQUIRE_FALSE(LoggerFactory::logFilePath().has_value());

    QFile::setPermissions(sandbox.path(), QFileDevice::ReadOwner |
                                              QFileDevice::WriteOwner |
                                              QFileDevice::ExeOwner);
}
