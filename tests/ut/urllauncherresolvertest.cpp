
#include <memory>

#include <catch2/catch_test_macros.hpp>

#include <aide/applicationconfig.hpp>

#include "mockurllauncher.hpp"
#include "osurllauncher.hpp"
#include "urllauncherresolver.hpp"

using aide::ApplicationConfig;
using aide::core::OsUrlLauncher;
using aide::core::UrlLauncherResolver;
using aide::tests::MockUrlLauncher;

TEST_CASE("A UrlLauncherResolver", "[UrlLauncherResolver]")
{
    SECTION(
        "resolves to the default OS-backed launcher when the config has "
        "no override")
    {
        const ApplicationConfig config;

        const auto resolved = UrlLauncherResolver::resolve(config);

        REQUIRE(dynamic_cast<OsUrlLauncher*>(resolved.get()) != nullptr);
    }

    SECTION("resolves to the config's overriding launcher when one is set")
    {
        ApplicationConfig config;
        const auto launcher = std::make_shared<MockUrlLauncher>();
        config.setUrlLauncher(launcher);

        const auto resolved = UrlLauncherResolver::resolve(config);

        REQUIRE(resolved == launcher);
    }
}
