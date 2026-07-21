#include "platformiconbaseline.hpp"

#include <QIcon>

namespace aide
{
    const PlatformIconBaseline& platformIconBaseline()
    {
        static const PlatformIconBaseline baseline{
            .themeName   = QIcon::themeName(),
            .searchPaths = QIcon::themeSearchPaths()};
        return baseline;
    }
} // namespace aide
