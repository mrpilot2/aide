#ifndef AIDE_VCS_VCS_MANAGER_HPP
#define AIDE_VCS_VCS_MANAGER_HPP

#include <aide/vcsmanagerinterface.hpp>

namespace aide
{
    /**
     * @brief Walking-skeleton implementation of VcsManagerInterface (#170).
     *
     * Proves the compile-time (AIDE_ENABLE_VCS) and runtime
     * (ApplicationConfig::Feature::VersionControlIntegration) plumbing
     * works. No repository can be registered yet.
     */
    class VcsManager : public VcsManagerInterface
    {
    public:
        [[nodiscard]] std::vector<std::string> registeredRepositories()
            const override;
    };
} // namespace aide

#endif // AIDE_VCS_VCS_MANAGER_HPP
