
#include <memory>
#include <string>

#include <catch2/catch_test_macros.hpp>

#include <QString>
#include <QUrl>
#include <QUrlQuery>

#include "mockurllauncher.hpp"
#include "reportbugusecase.hpp"

using aide::core::ReportBugUseCase;
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

TEST_CASE("A ReportBugUseCase", "[ReportBugUseCase]")
{
    SECTION(
        "asks the launcher to open a bug.yml new-issue URL against aIDE's "
        "own repo")
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

    SECTION(
        "puts a paste-your-log placeholder and a separator before the "
        "diagnostic block, without leaking a log file path")
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
        const auto logs = query.queryItemValue("logs");

        REQUIRE(logs.startsWith("Paste your log here"));
        REQUIRE(logs.contains("---"));
        REQUIRE(logs.indexOf("---") <
                logs.indexOf("aIDE")); // separator precedes the build info
        REQUIRE_FALSE(logs.contains("Log file"));
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
