#ifndef AIDE_VCS_VCS_REPOSITORY_HPP
#define AIDE_VCS_VCS_REPOSITORY_HPP

#include <functional>
#include <optional>
#include <string>

#include <aide/loggerinterface.hpp>
#include <aide/vcsprocessrunnerinterface.hpp>
#include <aide/vcsrepositoryinterface.hpp>

namespace aide
{
    /**
     * @brief VcsRepositoryInterface implementation driving the git binary
     * through a VcsProcessRunnerInterface (#171).
     *
     * The git executable is resolved lazily on every operation via
     * gitExecutableResolver, so overriding it on the owning VcsManager after
     * this repository was created still takes effect.
     */
    class VcsRepository : public VcsRepositoryInterface
    {
    public:
        using GitExecutableResolver =
            std::function<std::optional<std::string>()>;

        VcsRepository(std::string rootPath, VcsProcessRunnerInterfacePtr runner,
                      GitExecutableResolver gitExecutableResolver,
                      LoggerPtr logger);

        void status(StatusCallback callback) override;

        void stage(const std::vector<std::string>& paths,
                   OperationCallback callback) override;

        void unstage(const std::vector<std::string>& paths,
                     OperationCallback callback) override;

        void commit(const std::string& message,
                    OperationCallback callback) override;

        [[nodiscard]] const std::string& rootPath() const override;

    private:
        // Resolves the git executable and runs it with arguments against
        // this repository's root, or short-circuits with GitNotFound if no
        // executable is available. onComplete always receives the classified
        // VcsError alongside the raw process result.
        void runGit(const std::vector<std::string>& arguments,
                    std::function<void(VcsError, VcsProcessResult)> onComplete);

        // Shared plumbing for stage/unstage/commit: runs arguments and
        // turns the result into a VcsOperationResult.
        void runOperation(const std::vector<std::string>& arguments,
                          OperationCallback callback);

        std::string m_rootPath;
        VcsProcessRunnerInterfacePtr m_runner;
        GitExecutableResolver m_gitExecutableResolver;
        LoggerPtr m_logger;
    };
} // namespace aide

#endif // AIDE_VCS_VCS_REPOSITORY_HPP
