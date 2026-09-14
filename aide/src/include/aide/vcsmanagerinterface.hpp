#ifndef AIDE_VCS_MANAGER_INTERFACE_HPP
#define AIDE_VCS_MANAGER_INTERFACE_HPP

#include <memory>
#include <string>
#include <vector>

namespace aide
{
    /**
     * @brief Entry point for aIDE's Git version control integration (#169).
     *
     * This is the walking skeleton (#170): it exists so that
     * Application::vcsManager() has something to return once the
     * VersionControlIntegration feature is enabled and the aide::vcs module
     * is compiled in. No repository can be registered yet.
     */
    class VcsManagerInterface
    {
    public:
        virtual ~VcsManagerInterface() = default;

        [[nodiscard]] virtual std::vector<std::string> registeredRepositories()
            const = 0;
    };

    using VcsManagerInterfacePtr = std::shared_ptr<VcsManagerInterface>;
} // namespace aide

#endif // AIDE_VCS_MANAGER_INTERFACE_HPP
