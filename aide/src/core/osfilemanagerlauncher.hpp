
#ifndef AIDE_OS_FILE_MANAGER_LAUNCHER_HPP
#define AIDE_OS_FILE_MANAGER_LAUNCHER_HPP

#include <optional>

#include "filemanagerlauncher.hpp"

namespace aide::core
{
    // Cross-platform implementation: opens the OS's default handler for a
    // directory. The display name is hardcoded on Windows ("File Explorer")
    // and macOS ("Finder"), the two platforms where it is deterministic. On
    // Linux, where no single default exists, it is resolved on first use by
    // querying the desktop's configured default directory handler and
    // parsing its localized name, then cached; any failure along the way
    // (and every other platform) falls back to the generic "File Manager"
    // label.
    class OsFileManagerLauncher : public FileManagerLauncher
    {
    public:
        [[nodiscard]] std::string displayName() const override;

        bool openDirectory(const std::string& path) const override;

    private:
        mutable std::optional<std::string> m_cachedDisplayName;
    };
} // namespace aide::core

#endif // AIDE_OS_FILE_MANAGER_LAUNCHER_HPP
