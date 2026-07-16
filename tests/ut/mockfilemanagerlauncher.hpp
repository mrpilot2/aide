#ifndef AIDE_MOCK_FILE_MANAGER_LAUNCHER_HPP
#define AIDE_MOCK_FILE_MANAGER_LAUNCHER_HPP

#include <optional>
#include <string>

#include "filemanagerlauncher.hpp"

namespace aide::tests
{
    class MockFileManagerLauncher : public aide::core::FileManagerLauncher
    {
    public:
        bool openDirectoryReturnValue{true};
        mutable std::optional<std::string> lastRequestedDirectory;

        [[nodiscard]] std::string displayName() const override;

        bool openDirectory(const std::string& path) const override;
    };
} // namespace aide::tests

#endif // AIDE_MOCK_FILE_MANAGER_LAUNCHER_HPP
