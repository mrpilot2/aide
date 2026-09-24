#ifndef AIDE_VCS_VCS_MANAGER_HPP
#define AIDE_VCS_VCS_MANAGER_HPP

#include <map>
#include <memory>
#include <optional>
#include <string>

#include <aide/loggerinterface.hpp>
#include <aide/vcsmanagerinterface.hpp>
#include <aide/vcsprocessrunnerinterface.hpp>

namespace aide
{
    /**
     * @brief VcsManagerInterface implementation (#171).
     *
     * Resolves the git executable via PATH lookup by default
     * (QStandardPaths::findExecutable), or an explicit override set via
     * setGitExecutablePath(). Every VcsRepository it hands out re-resolves
     * the executable on each operation, so an override set after a
     * repository was registered still applies to it. The override is
     * shared by reference (not captured as `this`) so a repository handle
     * that outlives this VcsManager still resolves safely instead of
     * dereferencing a dangling manager.
     */
    class VcsManager : public VcsManagerInterface
    {
    public:
        VcsManager();
        explicit VcsManager(VcsProcessRunnerInterfacePtr runner,
                            LoggerPtr logger = nullptr);

        [[nodiscard]] std::vector<std::string> registeredRepositories()
            const override;

        VcsRepositoryInterfacePtr addRepository(
            const std::string& path) override;

        void setGitExecutablePath(std::string path) override;

        [[nodiscard]] std::optional<std::string> gitExecutablePath()
            const override;

    private:
        VcsProcessRunnerInterfacePtr m_runner;
        LoggerPtr m_logger;
        std::shared_ptr<std::optional<std::string>> m_gitExecutableOverride{
            std::make_shared<std::optional<std::string>>(std::nullopt)};
        std::map<std::string, VcsRepositoryInterfacePtr> m_repositories;
    };
} // namespace aide

#endif // AIDE_VCS_VCS_MANAGER_HPP
