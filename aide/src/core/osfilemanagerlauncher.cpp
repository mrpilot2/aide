
#include "osfilemanagerlauncher.hpp"

#include <QDesktopServices>
#include <QString>
#include <QUrl>

using aide::core::OsFileManagerLauncher;

std::string OsFileManagerLauncher::displayName() const
{
    return "File Manager";
}

bool OsFileManagerLauncher::openDirectory(const std::string& path) const
{
    return QDesktopServices::openUrl(
        QUrl::fromLocalFile(QString::fromStdString(path)));
}
