#include <catch2/catch_test_macros.hpp>

#include <QIcon>

#include "platformiconbaseline.hpp"

using aide::PlatformIconBaseline;
using aide::platformIconBaseline;

TEST_CASE("platformIconBaseline survives a later QIcon theme override",
          "[PlatformIconBaseline]")
{
    // A copy, not a reference: platformIconBaseline() returns a reference to
    // the same static storage on every call, so a reference here would alias
    // afterOverride below and the comparison would pass trivially even if the
    // "capture once" behavior were broken.
    // NOLINTNEXTLINE(performance-unnecessary-copy-initialization)
    const PlatformIconBaseline captured = platformIconBaseline();

    QIcon::setThemeName("some-other-theme-entirely");
    QIcon::setThemeSearchPaths({"/some/other/search/path"});

    const auto& afterOverride = platformIconBaseline();
    REQUIRE(afterOverride.themeName == captured.themeName);
    REQUIRE(afterOverride.searchPaths == captured.searchPaths);
}
