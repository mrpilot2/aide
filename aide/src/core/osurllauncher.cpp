
#include "osurllauncher.hpp"

#include <string>

#include <QDesktopServices>
#include <QString>
#include <QUrl>

using aide::core::OsUrlLauncher;

bool OsUrlLauncher::openUrl(const std::string& url) const
{
    return QDesktopServices::openUrl(QUrl(QString::fromStdString(url)));
}
