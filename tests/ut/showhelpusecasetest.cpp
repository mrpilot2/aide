#include <memory>
#include <string>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include <QUrl>

#include <aide/applicationconfig.hpp>
#include <aide/helptopicid.hpp>
#include <aide/hierarchicalid.hpp>
#include <aide/settings/settingspageregistry.hpp>

#include "mocksettingspage.hpp"
#include "mockurllauncher.hpp"
#include "recordinglogger.hpp"
#include "showhelpusecase.hpp"

using aide::ApplicationConfig;
using aide::HelpTopicId;
using aide::core::SettingsPageRegistry;
using aide::core::ShowHelpUseCase;
using aide::test::MockSettingsPage;
using aide::test::MockUrlLauncher;
using aide::test::RecordingLogger;

using Feature = ApplicationConfig::Feature;

namespace
{
    struct Fixture
    {
        std::shared_ptr<MockUrlLauncher> launcher{
            std::make_shared<MockUrlLauncher>()};
        std::shared_ptr<RecordingLogger> logger{
            std::make_shared<RecordingLogger>()};
        SettingsPageRegistry registry;

        [[nodiscard]] ShowHelpUseCase useCase(
            const ApplicationConfig& config) const
        {
            return {config, launcher, logger, registry};
        }

        void addPageWithTopic(const char* group, const char* topic)
        {
            auto page = std::make_shared<MockSettingsPage>(
                aide::HierarchicalId("Page")(group));
            page->setHelpTopicId(HelpTopicId{topic});
            registry.addPage(page);
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
        REQUIRE(fixture.logger->lines().size() == 1);
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
    REQUIRE(fixture.logger->lines().empty());
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

TEST_CASE("Context Help opens the resolved topic", "[ShowHelpUseCase]")
{
    const Fixture fixture;
    const auto useCase = fixture.useCase(configWithBase("https://x/help"));

    REQUIRE(useCase.showContextHelp(HelpTopicId{"workspace"}));
    REQUIRE(fixture.launcher->lastRequestedUrl ==
            std::string("https://x/help/workspace/"));
}

TEST_CASE("Context Help falls back to Help Contents", "[ShowHelpUseCase]")
{
    const Fixture fixture;

    SECTION("when no topic was resolved")
    {
        const auto useCase = fixture.useCase(configWithBase("https://x/help"));

        REQUIRE(useCase.showContextHelp(HelpTopicId{}));
        REQUIRE(fixture.launcher->lastRequestedUrl ==
                std::string("https://x/help/"));
    }

    SECTION("when the resolved topic is disabled")
    {
        const auto useCase =
            fixture.useCase(configWithBase("https://x/help")
                                .disableHelpTopic(HelpTopicId{"disabled"}));

        REQUIRE(useCase.showContextHelp(HelpTopicId{"disabled"}));
        REQUIRE(fixture.launcher->lastRequestedUrl ==
                std::string("https://x/help/"));
    }
}

TEST_CASE("Context Help with help inactive opens nothing", "[ShowHelpUseCase]")
{
    const Fixture fixture;
    const auto useCase = fixture.useCase(ApplicationConfig{});

    REQUIRE_FALSE(useCase.showContextHelp(HelpTopicId{"topic"}));
    REQUIRE_FALSE(fixture.launcher->lastRequestedUrl.has_value());
}

TEST_CASE(
    "Declared help topics are the settings pages' topics, sorted and "
    "de-duplicated",
    "[ShowHelpUseCase]")
{
    Fixture fixture;
    fixture.addPageWithTopic("1", "b");
    fixture.addPageWithTopic("2", "a");
    fixture.addPageWithTopic("3", "b");
    fixture.addPageWithTopic("4", "");

    const auto useCase = fixture.useCase(ApplicationConfig{});

    REQUIRE(useCase.declaredHelpTopics() ==
            std::vector{HelpTopicId{"a"}, HelpTopicId{"b"}});
}

TEST_CASE("Declared help topics drop disabled topics and apply remaps",
          "[ShowHelpUseCase]")
{
    Fixture fixture;
    fixture.addPageWithTopic("1", "a");
    fixture.addPageWithTopic("2", "b");
    fixture.addPageWithTopic("3", "c");

    SECTION("a disabled topic is dropped")
    {
        const auto useCase = fixture.useCase(
            ApplicationConfig{}.disableHelpTopic(HelpTopicId{"b"}));

        REQUIRE(useCase.declaredHelpTopics() ==
                std::vector{HelpTopicId{"a"}, HelpTopicId{"c"}});
    }

    SECTION(
        "disabling is checked against the declared topic, not its "
        "remap target")
    {
        const auto useCase = fixture.useCase(
            ApplicationConfig{}
                .remapHelpTopic(HelpTopicId{"a"}, HelpTopicId{"z"})
                .disableHelpTopic(HelpTopicId{"z"}));

        REQUIRE(useCase.declaredHelpTopics() == std::vector{HelpTopicId{"b"},
                                                            HelpTopicId{"c"},
                                                            HelpTopicId{"z"}});
    }

    SECTION("a remapped topic is replaced by its target, one step only")
    {
        const auto useCase = fixture.useCase(
            ApplicationConfig{}
                .remapHelpTopic(HelpTopicId{"a"}, HelpTopicId{"x/y"})
                .remapHelpTopic(HelpTopicId{"x/y"}, HelpTopicId{"q"}));

        REQUIRE(useCase.declaredHelpTopics() ==
                std::vector{HelpTopicId{"b"}, HelpTopicId{"c"},
                            HelpTopicId{"x/y"}});
    }

    SECTION("topics remapped onto one target appear once")
    {
        const auto useCase = fixture.useCase(
            ApplicationConfig{}
                .remapHelpTopic(HelpTopicId{"a"}, HelpTopicId{"c"})
                .remapHelpTopic(HelpTopicId{"b"}, HelpTopicId{"c"}));

        REQUIRE(useCase.declaredHelpTopics() == std::vector{HelpTopicId{"c"}});
    }
}

TEST_CASE("Declared help topics include the config's declared widget topics",
          "[ShowHelpUseCase]")
{
    Fixture fixture;
    fixture.addPageWithTopic("1", "page");

    const auto useCase = fixture.useCase(
        ApplicationConfig{}
            .declareHelpTopic(HelpTopicId{"widget"})
            .declareHelpTopic(HelpTopicId{"page"})
            .declareHelpTopic(HelpTopicId{"off"})
            .declareHelpTopic(HelpTopicId{"old"})
            .disableHelpTopic(HelpTopicId{"off"})
            .remapHelpTopic(HelpTopicId{"old"}, HelpTopicId{"new"}));

    REQUIRE(useCase.declaredHelpTopics() == std::vector{HelpTopicId{"new"},
                                                        HelpTopicId{"page"},
                                                        HelpTopicId{"widget"}});
}

TEST_CASE("Context Help on an undeclared topic warns once", "[ShowHelpUseCase]")
{
    Fixture fixture;
    fixture.addPageWithTopic("1", "page");
    const auto useCase =
        fixture.useCase(configWithBase("https://x/help")
                            .declareHelpTopic(HelpTopicId{"widget"})
                            .declareHelpTopic(HelpTopicId{"off"})
                            .disableHelpTopic(HelpTopicId{"off"}));

    SECTION("and still opens it")
    {
        REQUIRE(useCase.showContextHelp(HelpTopicId{"undeclared"}));
        REQUIRE(fixture.logger->lines().size() == 1);
        REQUIRE(fixture.logger->hasWarning());
        REQUIRE(fixture.logger->lines().front().message.find("undeclared") !=
                std::string::npos);
    }

    SECTION("and not again when triggered a second time")
    {
        useCase.showContextHelp(HelpTopicId{"undeclared"});
        useCase.showContextHelp(HelpTopicId{"undeclared"});

        REQUIRE(fixture.logger->lines().size() == 1);
    }

    SECTION(
        "but not on a declared, page or disabled declared topic, or on "
        "none")
    {
        useCase.showContextHelp(HelpTopicId{"widget"});
        useCase.showContextHelp(HelpTopicId{"page"});
        useCase.showContextHelp(HelpTopicId{"off"});
        useCase.showContextHelp(HelpTopicId{});

        REQUIRE(fixture.logger->lines().empty());
    }
}
