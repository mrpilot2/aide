
#ifndef AIDE_OS_FILE_MANAGER_LAUNCHER_HPP
#define AIDE_OS_FILE_MANAGER_LAUNCHER_HPP

#include "filemanagerlauncher.hpp"

namespace aide::core
{
    // Baseline cross-platform implementation: opens the OS's default handler
    // for a directory and always reports the generic "File Manager" display
    // name. Platform-specific exact naming (hardcoded Windows/macOS labels,
    // Linux runtime resolution) is added by later, focused tickets without
    // introducing a new abstraction.
    class OsFileManagerLauncher : public FileManagerLauncher
    {
    public:
        [[nodiscard]] std::string displayName() const override;

        bool openDirectory(const std::string& path) const override;
    };
} // namespace aide::core

#endif // AIDE_OS_FILE_MANAGER_LAUNCHER_HPP
