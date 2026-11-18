#include "desktopurllauncher.hpp"

#include <string>

#include <QDesktopServices>
#include <QString>
#include <QUrl>

using demo::DesktopUrlLauncher;

bool DesktopUrlLauncher::openUrl(const std::string& url) const
{
    return QDesktopServices::openUrl(QUrl(QString::fromStdString(url)));
}
