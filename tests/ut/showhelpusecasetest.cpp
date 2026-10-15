#include <memory>
#include <string>

#include <catch2/catch_test_macros.hpp>

#include <QUrl>

#include <aide/applicationconfig.hpp>
#include <aide/helptopicid.hpp>

#include "mockurllauncher.hpp"
#include "showhelpusecase.hpp"

using aide::ApplicationConfig;
using aide::HelpTopicId;
using aide::core::ShowHelpUseCase;
using aide::tests::MockUrlLauncher;

using Feature = ApplicationConfig::Feature;

namespace
{
    // Counts log lines, without depending on the real spdlog-backed logger.
    class RecordingLogger : public aide::LoggerInterface
    {
    public:
        mutable int lineCount{0};

        void flush() override {}
        void setLevel(aide::LogLevel /*level*/) override {}

    private:
        void doLogTrace(std::string_view /*message*/) const override
        {
            ++lineCount;
        }
        void doLogDebug(std::string_view /*message*/) const override
        {
            ++lineCount;
        }
        void doLogInfo(std::string_view /*message*/) const override
        {
            ++lineCount;
        }
        void doLogWarn(std::string_view /*message*/) const override
        {
            ++lineCount;
        }
        void doLogError(std::string_view /*message*/) const override
        {
            ++lineCount;
        }
        void doLogCritical(std::string_view /*message*/) const override
        {
            ++lineCount;
        }
    };

    struct Fixture
    {
        std::shared_ptr<MockUrlLauncher> launcher{
            std::make_shared<MockUrlLauncher>()};
        std::shared_ptr<RecordingLogger> logger{
            std::make_shared<RecordingLogger>()};

        [[nodiscard]] ShowHelpUseCase useCase(
            const ApplicationConfig& config) const
        {
            return {config, launcher, logger};
        }
    };

    ApplicationConfig configWithBase(const char* base)
    {
        ApplicationConfig config;
        config.setHelpBaseUrl(QUrl(base));
        return config;
    }
} // namespace

TEST_CASE("Showing a help topic launches <base>/<topic-id>/",
          "[ShowHelpUseCase]")
{
    const Fixture fixture;

    SECTION("when the base has no trailing slash")
    {
        const auto useCase = fixture.useCase(configWithBase("https://x/help"));

        REQUIRE(useCase.showHelp(HelpTopicId{"keymap"}));
        REQUIRE(fixture.launcher->lastRequestedUrl ==
                std::string("https://x/help/keymap/"));
    }

    SECTION("when the base has a trailing slash")
    {
        const auto useCase = fixture.useCase(configWithBase("https://x/help/"));

        REQUIRE(useCase.showHelp(HelpTopicId{"keymap"}));
        REQUIRE(fixture.launcher->lastRequestedUrl ==
                std::string("https://x/help/keymap/"));
    }

    SECTION("for a nested topic id")
    {
        const auto useCase = fixture.useCase(configWithBase("https://x/help"));

        REQUIRE(useCase.showHelp(HelpTopicId{"settings/keymap"}));
        REQUIRE(fixture.launcher->lastRequestedUrl ==
                std::string("https://x/help/settings/keymap/"));
    }
}

TEST_CASE("Help with the feature on but no base URL is inactive",
          "[ShowHelpUseCase]")
{
    const Fixture fixture;
    const auto useCase = fixture.useCase(ApplicationConfig{});

    REQUIRE_FALSE(useCase.isHelpAvailable(HelpTopicId{"keymap"}));
    REQUIRE_FALSE(useCase.showHelp(HelpTopicId{"keymap"}));
    REQUIRE_FALSE(useCase.showHelpContents());
    REQUIRE_FALSE(fixture.launcher->lastRequestedUrl.has_value());

    SECTION("and says so in a single log line")
    {
        REQUIRE(fixture.logger->lineCount == 1);
    }
}

TEST_CASE("Help with the feature off is inactive even with a base URL",
          "[ShowHelpUseCase]")
{
    const Fixture fixture;
    const auto useCase = fixture.useCase(
        configWithBase("https://x/help").setEnabled(Feature::Help, false));

    REQUIRE_FALSE(useCase.isHelpAvailable(HelpTopicId{"keymap"}));
    REQUIRE_FALSE(useCase.showHelp(HelpTopicId{"keymap"}));
    REQUIRE_FALSE(useCase.showHelpContents());
    REQUIRE_FALSE(fixture.launcher->lastRequestedUrl.has_value());
    REQUIRE(fixture.logger->lineCount == 0);
}

TEST_CASE("An empty help topic is not available", "[ShowHelpUseCase]")
{
    const Fixture fixture;
    const auto useCase = fixture.useCase(configWithBase("https://x/help"));

    REQUIRE_FALSE(useCase.isHelpAvailable(HelpTopicId{}));
    REQUIRE_FALSE(useCase.showHelp(HelpTopicId{}));
    REQUIRE_FALSE(fixture.launcher->lastRequestedUrl.has_value());
}

TEST_CASE("A disabled help topic is not available", "[ShowHelpUseCase]")
{
    const Fixture fixture;
    const auto useCase =
        fixture.useCase(configWithBase("https://x/help")
                            .disableHelpTopic(HelpTopicId{"keymap"}));

    REQUIRE_FALSE(useCase.isHelpAvailable(HelpTopicId{"keymap"}));
    REQUIRE_FALSE(useCase.showHelp(HelpTopicId{"keymap"}));
    REQUIRE_FALSE(fixture.launcher->lastRequestedUrl.has_value());
    REQUIRE(useCase.isHelpAvailable(HelpTopicId{"appearance"}));
}

TEST_CASE("Showing Help Contents launches the base URL root",
          "[ShowHelpUseCase]")
{
    const Fixture fixture;
    const auto useCase = fixture.useCase(configWithBase("https://x/help"));

    REQUIRE(useCase.showHelpContents());
    REQUIRE(fixture.launcher->lastRequestedUrl ==
            std::string("https://x/help/"));
}

TEST_CASE("Showing help reports a launcher failure", "[ShowHelpUseCase]")
{
    const Fixture fixture;
    fixture.launcher->openUrlReturnValue = false;
    const auto useCase = fixture.useCase(configWithBase("https://x/help"));

    REQUIRE_FALSE(useCase.showHelp(HelpTopicId{"keymap"}));
    REQUIRE_FALSE(useCase.showHelpContents());
}

TEST_CASE("A remapped help topic opens its target", "[ShowHelpUseCase]")
{
    const Fixture fixture;
    const auto useCase = fixture.useCase(
        configWithBase("https://x/help")
            .remapHelpTopic(HelpTopicId{"keymap"}, HelpTopicId{"keys"}));

    REQUIRE(useCase.showHelp(HelpTopicId{"keymap"}));
    REQUIRE(fixture.launcher->lastRequestedUrl ==
            std::string("https://x/help/keys/"));
}

TEST_CASE("Help topic remaps apply one step only", "[ShowHelpUseCase]")
{
    const Fixture fixture;
    const auto useCase = fixture.useCase(
        configWithBase("https://x/help")
            .remapHelpTopic(HelpTopicId{"a"}, HelpTopicId{"b"})
            .remapHelpTopic(HelpTopicId{"b"}, HelpTopicId{"c"}));

    REQUIRE(useCase.showHelp(HelpTopicId{"a"}));
    REQUIRE(fixture.launcher->lastRequestedUrl ==
            std::string("https://x/help/b/"));
}

TEST_CASE("Disabling a help topic is checked before remapping",
          "[ShowHelpUseCase]")
{
    const Fixture fixture;
    const auto useCase =
        fixture.useCase(configWithBase("https://x/help")
                            .remapHelpTopic(HelpTopicId{"a"}, HelpTopicId{"b"})
                            .disableHelpTopic(HelpTopicId{"a"}));

    REQUIRE_FALSE(useCase.isHelpAvailable(HelpTopicId{"a"}));
    REQUIRE_FALSE(useCase.showHelp(HelpTopicId{"a"}));
    REQUIRE_FALSE(fixture.launcher->lastRequestedUrl.has_value());
}

TEST_CASE("Remapping onto a disabled help topic does not disable the topic",
          "[ShowHelpUseCase]")
{
    const Fixture fixture;
    const auto useCase =
        fixture.useCase(configWithBase("https://x/help")
                            .remapHelpTopic(HelpTopicId{"a"}, HelpTopicId{"b"})
                            .disableHelpTopic(HelpTopicId{"b"}));

    REQUIRE(useCase.isHelpAvailable(HelpTopicId{"a"}));
    REQUIRE(useCase.showHelp(HelpTopicId{"a"}));
    REQUIRE(fixture.launcher->lastRequestedUrl ==
            std::string("https://x/help/b/"));
}

TEST_CASE("The last help topic remap registration wins", "[ShowHelpUseCase]")
{
    const Fixture fixture;
    const auto useCase = fixture.useCase(
        configWithBase("https://x/help")
            .remapHelpTopic(HelpTopicId{"a"}, HelpTopicId{"b"})
            .remapHelpTopic(HelpTopicId{"a"}, HelpTopicId{"c"}));

    REQUIRE(useCase.showHelp(HelpTopicId{"a"}));
    REQUIRE(fixture.launcher->lastRequestedUrl ==
            std::string("https://x/help/c/"));
}

TEST_CASE("A help topic remapped onto an empty id is not available",
          "[ShowHelpUseCase]")
{
    const Fixture fixture;
    const auto useCase =
        fixture.useCase(configWithBase("https://x/help")
                            .remapHelpTopic(HelpTopicId{"a"}, HelpTopicId{}));

    REQUIRE_FALSE(useCase.isHelpAvailable(HelpTopicId{"a"}));
    REQUIRE_FALSE(useCase.showHelp(HelpTopicId{"a"}));
}
