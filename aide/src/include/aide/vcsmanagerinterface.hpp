#ifndef AIDE_VCS_MANAGER_INTERFACE_HPP
#define AIDE_VCS_MANAGER_INTERFACE_HPP

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include <aide/vcsrepositoryinterface.hpp>

namespace aide
{
    /**
     * @brief Entry point for aIDE's Git version control integration (#169).
     *
     * Registers repository roots (#171) and resolves the `git` executable
     * used to operate on them, falling back to a PATH lookup unless
     * overridden via setGitExecutablePath().
     */
    class VcsManagerInterface
    {
    public:
        virtual ~VcsManagerInterface() = default;

        [[nodiscard]] virtual std::vector<std::string> registeredRepositories()
            const = 0;

        // Registers path as a repository root and returns a handle to it.
        // The handle is created eagerly; whether path actually resolves to
        // a usable git repository is only known once an operation is run
        // against it (surfaced as VcsError::NotARepository).
        virtual VcsRepositoryInterfacePtr addRepository(
            const std::string& path) = 0;

        // Overrides the git executable used for all future operations,
        // in place of the default PATH lookup.
        virtual void setGitExecutablePath(std::string path) = 0;

        // The resolved git executable path, or empty if none could be
        // resolved (surfaced as VcsError::GitNotFound on operations).
        [[nodiscard]] virtual std::optional<std::string> gitExecutablePath()
            const = 0;
    };

    using VcsManagerInterfacePtr = std::shared_ptr<VcsManagerInterface>;
} // namespace aide

#endif // AIDE_VCS_MANAGER_INTERFACE_HPP
