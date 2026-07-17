#ifndef AIDE_PLATFORM_ICON_BASELINE_HPP
#define AIDE_PLATFORM_ICON_BASELINE_HPP

#include <QString>
#include <QStringList>

namespace aide
{
    // The platform's icon theme name and search paths as they were before
    // AppearanceManager first overrides them via QIcon::setThemeName() /
    // setThemeSearchPaths(). Qt's icon theme is process-global, so this is
    // captured once for the life of the process - not once per
    // AppearanceManager instance. See docs/adr/0001-platform-icon-baseline-
    // captured-once-per-process.md for why per-instance capture would be
    // wrong, not just less deep.
    struct PlatformIconBaseline
    {
        QString themeName{};
        QStringList searchPaths{};
    };

    const PlatformIconBaseline& platformIconBaseline();
} // namespace aide

#endif // AIDE_PLATFORM_ICON_BASELINE_HPP
