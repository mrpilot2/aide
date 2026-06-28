#ifndef AIDE_THEME_HPP
#define AIDE_THEME_HPP

#include <QPalette>
#include <QString>
#include <QStringList>

namespace aide
{
    struct Theme
    {
        QString name;
        QPalette palette;
        QString iconThemeName;
        QStringList iconSearchPaths;
    };
} // namespace aide

#endif // AIDE_THEME_HPP
