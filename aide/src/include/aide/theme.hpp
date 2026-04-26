#ifndef AIDE_THEME_HPP
#define AIDE_THEME_HPP

#include <QPalette>
#include <QString>

namespace aide
{
    struct Theme
    {
        QString name;
        QPalette palette;
        QString iconThemeName;
        QString iconSearchPath;
    };
} // namespace aide

#endif // AIDE_THEME_HPP
