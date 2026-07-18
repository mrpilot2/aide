#include <memory>

#include <catch2/catch_test_macros.hpp>

#include <aide/applicationconfig.hpp>

#include "mockurllauncher.hpp"

using aide::ApplicationConfig;
using aide::tests::MockUrlLauncher;

using Feature = ApplicationConfig::Feature;

TEST_CASE("An application config")
{
    SECTION("enables every feature by default")
    {
        const ApplicationConfig config;

        REQUIRE(config.isEnabled(Feature::ViewFullscreenAction));
    }

    SECTION("reflects an explicit override")
    {
        ApplicationConfig config;

        config.setEnabled(Feature::ViewFullscreenAction, false);

        REQUIRE_FALSE(config.isEnabled(Feature::ViewFullscreenAction));
    }

    SECTION("re-enabling a feature restores it")
    {
        ApplicationConfig config;

        config.setEnabled(Feature::ViewFullscreenAction, false);
        config.setEnabled(Feature::ViewFullscreenAction, true);

        REQUIRE(config.isEnabled(Feature::ViewFullscreenAction));
    }

    SECTION("setEnabled chains fluently")
    {
        ApplicationConfig config;

        REQUIRE(&config.setEnabled(Feature::ViewFullscreenAction, false) ==
                &config);
    }

    SECTION("defaults the show-log-in-file-manager feature to disabled")
    {
        const ApplicationConfig config;

        REQUIRE_FALSE(config.isEnabled(Feature::ShowLogInFileManagerAction));
    }

    SECTION(
        "reflects an explicit opt-in for the show-log-in-file-manager "
        "feature")
    {
        ApplicationConfig config;

        config.setEnabled(Feature::ShowLogInFileManagerAction, true);

        REQUIRE(config.isEnabled(Feature::ShowLogInFileManagerAction));
    }

    SECTION("defaults the report-bug feature to disabled")
    {
        const ApplicationConfig config;

        REQUIRE_FALSE(config.isEnabled(Feature::ReportBugAction));
    }

    SECTION("reflects an explicit opt-in for the report-bug feature")
    {
        ApplicationConfig config;

        config.setEnabled(Feature::ReportBugAction, true);

        REQUIRE(config.isEnabled(Feature::ReportBugAction));
    }

    SECTION("a copy carries overrides independently of the original")
    {
        ApplicationConfig original;
        original.setEnabled(Feature::ViewFullscreenAction, false);

        ApplicationConfig copy{original};
        copy.setEnabled(Feature::ViewFullscreenAction, true);

        REQUIRE_FALSE(original.isEnabled(Feature::ViewFullscreenAction));
        REQUIRE(copy.isEnabled(Feature::ViewFullscreenAction));
    }

    SECTION("copy assignment carries overrides independently of the original")
    {
        ApplicationConfig original;
        original.setEnabled(Feature::ViewFullscreenAction, false);

        ApplicationConfig assigned;
        assigned = original;
        assigned.setEnabled(Feature::ViewFullscreenAction, true);

        REQUIRE_FALSE(original.isEnabled(Feature::ViewFullscreenAction));
        REQUIRE(assigned.isEnabled(Feature::ViewFullscreenAction));
    }

    SECTION("has no URL launcher override by default")
    {
        const ApplicationConfig config;

        REQUIRE(config.urlLauncher() == nullptr);
    }

    SECTION("reflects an explicit URL launcher override")
    {
        ApplicationConfig config;
        const auto launcher = std::make_shared<MockUrlLauncher>();

        config.setUrlLauncher(launcher);

        REQUIRE(config.urlLauncher() == launcher);
    }

    SECTION("setUrlLauncher chains fluently")
    {
        ApplicationConfig config;

        REQUIRE(&config.setUrlLauncher(std::make_shared<MockUrlLauncher>()) ==
                &config);
    }

    SECTION(
        "a copy carries the URL launcher override independently of the "
        "original")
    {
        ApplicationConfig original;
        const auto launcher = std::make_shared<MockUrlLauncher>();
        original.setUrlLauncher(launcher);

        ApplicationConfig copy{original};
        copy.setUrlLauncher(nullptr);

        REQUIRE(original.urlLauncher() == launcher);
        REQUIRE(copy.urlLauncher() == nullptr);
    }
}
