
#include "osfilemanagerlauncher.hpp"

#include <optional>
#include <string>

#include <QDesktopServices>
#include <QString>
#include <QUrl>

#ifdef Q_OS_LINUX
#include <QFile>
#include <QIODevice>
#include <QLocale>
#include <QProcess>
#include <QStandardPaths>

#include "desktopentrynameresolver.hpp"
#endif

using aide::core::OsFileManagerLauncher;

namespace
{
#ifdef Q_OS_LINUX
    constexpr int kXdgMimeQueryTimeoutMs = 1000;

    // Queries the desktop's configured default handler for directories and
    // resolves its localized display name. Any failure (query tool
    // missing/hung, handler's .desktop file not found in the standard
    // application-metadata search paths, or no usable Name parsed) yields
    // std::nullopt so the caller can fall back to the generic label.
    std::optional<std::string> resolveLinuxFileManagerDisplayName()
    {
        QProcess process;
        process.start("xdg-mime", {"query", "default", "inode/directory"});
        if (!process.waitForFinished(kXdgMimeQueryTimeoutMs)) {
            process.kill();
            return std::nullopt;
        }
        if (process.exitStatus() != QProcess::NormalExit ||
            process.exitCode() != 0) {
            return std::nullopt;
        }

        const auto desktopId =
            QString::fromUtf8(process.readAllStandardOutput()).trimmed();
        if (desktopId.isEmpty()) { return std::nullopt; }

        const auto desktopFilePath = QStandardPaths::locate(
            QStandardPaths::ApplicationsLocation, desktopId);
        if (desktopFilePath.isEmpty()) { return std::nullopt; }

        QFile desktopFile(desktopFilePath);
        if (!desktopFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            return std::nullopt;
        }

        return aide::core::DesktopEntryNameResolver::resolve(
            QString::fromUtf8(desktopFile.readAll()).toStdString(),
            QLocale::system().name().toStdString());
    }
#endif

    std::string resolveDisplayName()
    {
#if defined(Q_OS_WIN)
        return "File Explorer";
#elif defined(Q_OS_MACOS)
        return "Finder";
#elif defined(Q_OS_LINUX)
        if (const auto resolvedName = resolveLinuxFileManagerDisplayName();
            resolvedName.has_value()) {
            return *resolvedName;
        }
        return "File Manager";
#else
        return "File Manager";
#endif
    }
} // namespace

std::string OsFileManagerLauncher::displayName() const
{
    if (!m_cachedDisplayName.has_value()) {
        m_cachedDisplayName = resolveDisplayName();
    }
    return *m_cachedDisplayName;
}

bool OsFileManagerLauncher::openDirectory(const std::string& path) const
{
    return QDesktopServices::openUrl(
        QUrl::fromLocalFile(QString::fromStdString(path)));
}
