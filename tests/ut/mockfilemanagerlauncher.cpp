#include "mockfilemanagerlauncher.hpp"

using aide::tests::MockFileManagerLauncher;

std::string MockFileManagerLauncher::displayName() const
{
    return "File Manager";
}

bool MockFileManagerLauncher::openDirectory(const std::string& path) const
{
    lastRequestedDirectory = path;
    return openDirectoryReturnValue;
}
