
#ifndef AIDE_FILE_MANAGER_LAUNCHER_HPP
#define AIDE_FILE_MANAGER_LAUNCHER_HPP

#include <memory>
#include <string>

namespace aide::core
{
    // Bundles both responsibilities of the "Show Log in <File Manager>"
    // feature in one interface: producing a human-facing display name for
    // the current platform's file manager, and opening a given directory in
    // it. Kept singular (rather than two interfaces) since the two concerns
    // are conceptually one feature and always share one concrete
    // implementation and one test double.
    class FileManagerLauncher
    {
    public:
        virtual ~FileManagerLauncher() = default;

        // Human-facing display name for the current platform's file
        // manager, e.g. "File Manager", "Finder", "File Explorer", or a
        // desktop's actual default file manager name.
        [[nodiscard]] virtual std::string displayName() const = 0;

        // Opens the given directory in the platform's file manager. Returns
        // whether the OS reported success.
        virtual bool openDirectory(const std::string& path) const = 0;
    };

    using FileManagerLauncherPtr = std::shared_ptr<FileManagerLauncher>;
} // namespace aide::core

#endif // AIDE_FILE_MANAGER_LAUNCHER_HPP
