#ifndef AIDE_COLOR_SCHEME_HPP
#define AIDE_COLOR_SCHEME_HPP

#include <QMetaType>

namespace aide
{
    enum class ColorScheme
    {
        Light,
        Dark
    };
} // namespace aide

Q_DECLARE_METATYPE(aide::ColorScheme)

#endif // AIDE_COLOR_SCHEME_HPP
