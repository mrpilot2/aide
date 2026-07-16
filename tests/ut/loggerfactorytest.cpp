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

using aide::core::LoggerFactory;

namespace
{
    std::string getFileContents(const std::string& fileName)
    {
        const std::ifstream logFile(fileName, std::ios::in);
        std::stringstream fileContent;
        fileContent << logFile.rdbuf();
        return fileContent.str();
    }

    // Restores an environment variable to whatever it was before the test
    // touched it, even if a REQUIRE() aborts the test case early.
    class EnvVarGuard
    {
    public:
        EnvVarGuard(std::string name, const QString& value)
            : m_name{std::move(name)}
            , m_hadPrevious{qEnvironmentVariableIsSet(m_name.c_str())}
            , m_previous{qgetenv(m_name.c_str())}
        {
            qputenv(m_name.c_str(), value.toUtf8());
        }

        EnvVarGuard(const EnvVarGuard&)            = delete;
        EnvVarGuard& operator=(const EnvVarGuard&) = delete;
        EnvVarGuard(EnvVarGuard&&)                 = delete;
        EnvVarGuard& operator=(EnvVarGuard&&)      = delete;

        ~EnvVarGuard()
        {
            if (m_hadPrevious) {
                qputenv(m_name.c_str(), m_previous);
            } else {
                qunsetenv(m_name.c_str());
            }
        }

    private:
        std::string m_name;
        bool m_hadPrevious;
        QByteArray m_previous;
    };

    // Qt's standard-paths test mode redirects CacheLocation (and friends)
    // under a sandbox so tests never touch the real user cache directory.
    class StandardPathsTestModeGuard
    {
    public:
        StandardPathsTestModeGuard()
        {
            QStandardPaths::setTestModeEnabled(true);
        }

        StandardPathsTestModeGuard(const StandardPathsTestModeGuard&) = delete;
        StandardPathsTestModeGuard& operator=(
            const StandardPathsTestModeGuard&)                   = delete;
        StandardPathsTestModeGuard(StandardPathsTestModeGuard&&) = delete;
        StandardPathsTestModeGuard& operator=(StandardPathsTestModeGuard&&) =
            delete;

        ~StandardPathsTestModeGuard()
        {
            QStandardPaths::setTestModeEnabled(false);
        }
    };
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
    const EnvVarGuard xdgCacheGuard("XDG_CACHE_HOME",
                                    sandbox.path() + "/cache");

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
    // writable location" for both the cache and the temp-dir fallback.
    REQUIRE(QFile::setPermissions(
        sandbox.path(), QFileDevice::ReadOwner | QFileDevice::ExeOwner));

    const StandardPathsTestModeGuard testModeGuard;
    const EnvVarGuard homeGuard("HOME", sandbox.path());
    const EnvVarGuard xdgCacheGuard("XDG_CACHE_HOME",
                                    sandbox.path() + "/cache");
    const EnvVarGuard tmpdirGuard("TMPDIR", sandbox.path() + "/tmp");

    REQUIRE_FALSE(LoggerFactory::logFilePath().has_value());

    QFile::setPermissions(sandbox.path(), QFileDevice::ReadOwner |
                                              QFileDevice::WriteOwner |
                                              QFileDevice::ExeOwner);
}
