
#include <memory>
#include <string>
#include <utility>

#include <catch2/catch_test_macros.hpp>

#include <QCoreApplication>
#include <QStandardPaths>
#include <QString>
#include <QTemporaryDir>
#include <QUrl>
#include <QUrlQuery>

#include <aide/logger/loggerfactory.hpp>

#include "mockurllauncher.hpp"
#include "reportbugusecase.hpp"
#include "standardpathstestguards.hpp"

using aide::core::LoggerFactory;
using aide::core::ReportBugUseCase;
using aide::test::BlockedStandardLocationGuard;
using aide::test::EnvVarGuard;
using aide::test::StandardPathsTestModeGuard;
using aide::tests::MockUrlLauncher;

namespace
{
    // Records whether a warning was logged, without depending on the real
    // spdlog-backed logger.
    class RecordingLogger : public aide::LoggerInterface
    {
    public:
        mutable bool warnCalled{false};

        void flush() override {}
        void setLevel(aide::LogLevel /*level*/) override {}

    private:
        void doLogTrace(std::string_view /*message*/) const override {}
        void doLogDebug(std::string_view /*message*/) const override {}
        void doLogInfo(std::string_view /*message*/) const override {}
        void doLogWarn(std::string_view /*message*/) const override
        {
            warnCalled = true;
        }
        void doLogError(std::string_view /*message*/) const override {}
        void doLogCritical(std::string_view /*message*/) const override {}
    };
} // namespace

TEST_CASE("A ReportBugUseCase with a resolvable log file path",
          "[ReportBugUseCase]")
{
    const QTemporaryDir sandbox;
    REQUIRE(sandbox.isValid());

    const StandardPathsTestModeGuard testModeGuard;
    const EnvVarGuard homeGuard("HOME", sandbox.path());

    QCoreApplication::setApplicationName("aide_report_bug_use_case_test");

    // Ensures LoggerFactory::logFilePath() has a resolved location to work
    // with, matching the "consistent with a freshly constructed logger"
    // guarantee covered in loggerfactorytest.cpp.
    LoggerFactory::createLogger("report_bug_use_case_test");

    const auto resolvedLogFilePath = LoggerFactory::logFilePath();
    REQUIRE(resolvedLogFilePath.has_value());

    SECTION(
        "asks the launcher to open a bug.yml new-issue URL against "
        "aIDE's own repo")
    {
        const auto launcher = std::make_shared<MockUrlLauncher>();
        const auto logger   = std::make_shared<RecordingLogger>();

        const ReportBugUseCase useCase(launcher, logger);
        useCase.reportBug();

        REQUIRE(launcher->lastRequestedUrl.has_value());
        // NOLINTNEXTLINE(bugprone-unchecked-optional-access)
        const auto& requestedUrl = launcher->lastRequestedUrl.value();
        const QUrl url(QString::fromStdString(requestedUrl));

        REQUIRE(url.toString().startsWith(
            "https://github.com/mrpilot2/aide/issues/new"));

        const QUrlQuery query(url);
        REQUIRE(query.queryItemValue("template") == "bug.yml");
        REQUIRE_FALSE(query.queryItemValue("version").isEmpty());
        REQUIRE_FALSE(query.queryItemValue("os").isEmpty());
        REQUIRE_FALSE(query.queryItemValue("logs").isEmpty());
        REQUIRE_FALSE(logger->warnCalled);
    }

    SECTION("leaves the reporter-authored fields out of the URL")
    {
        const auto launcher = std::make_shared<MockUrlLauncher>();
        const auto logger   = std::make_shared<RecordingLogger>();

        const ReportBugUseCase useCase(launcher, logger);
        useCase.reportBug();

        REQUIRE(launcher->lastRequestedUrl.has_value());
        // NOLINTNEXTLINE(bugprone-unchecked-optional-access)
        const auto& requestedUrl = launcher->lastRequestedUrl.value();
        const QUrl url(QString::fromStdString(requestedUrl));
        const QUrlQuery query(url);

        REQUIRE(query.queryItemValue("contact").isEmpty());
        REQUIRE(query.queryItemValue("what-happened").isEmpty());
        REQUIRE(query.queryItemValue("reproduce").isEmpty());
        REQUIRE(query.queryItemValue("screenshot").isEmpty());
    }

    SECTION("logs a warning and nothing else when the launcher reports failure")
    {
        const auto launcher          = std::make_shared<MockUrlLauncher>();
        launcher->openUrlReturnValue = false;
        const auto logger            = std::make_shared<RecordingLogger>();

        const ReportBugUseCase useCase(launcher, logger);
        useCase.reportBug();

        REQUIRE(logger->warnCalled);
    }
}

TEST_CASE("A ReportBugUseCase with an unresolvable log file path",
          "[ReportBugUseCase]")
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

    const auto launcher = std::make_shared<MockUrlLauncher>();
    const auto logger   = std::make_shared<RecordingLogger>();

    const ReportBugUseCase useCase(launcher, logger);
    useCase.reportBug();

    SECTION("does not ask the launcher to open anything")
    {
        REQUIRE_FALSE(launcher->lastRequestedUrl.has_value());
    }

    SECTION("logs a warning")
    {
        REQUIRE(logger->warnCalled);
    }
}
