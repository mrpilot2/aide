#include "platformiconbaseline.hpp"

#include <QIcon>

namespace aide
{
    const PlatformIconBaseline& platformIconBaseline()
    {
        static const PlatformIconBaseline baseline{QIcon::themeName(),
                                                   QIcon::themeSearchPaths()};
        return baseline;
    }
} // namespace aide
