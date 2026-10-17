#ifndef AIDE_VCS_REPOSITORY_INTERFACE_HPP
#define AIDE_VCS_REPOSITORY_INTERFACE_HPP

#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <vector>

#include <aide/vcserror.hpp>

namespace aide
{
    /**
     * @brief Per-file change state, as reported by `git status`.
     */
    enum class VcsFileState : std::uint8_t
    {
        Unmodified,
        Added,
        Modified,
        Deleted,
        Renamed,
        Copied,
        Untracked,
        Ignored,
        Conflicted
    };

    /**
     * @brief One file's status, split into its staged (index) and unstaged
     * (work tree) state, mirroring the two columns of `git status
     * --porcelain`.
     */
    struct VcsFileStatus
    {
        std::string path;
        VcsFileState indexState{VcsFileState::Unmodified};
        VcsFileState workTreeState{VcsFileState::Unmodified};
    };

    struct VcsStatusResult
    {
        VcsError error{VcsError::None};
        std::vector<VcsFileStatus> entries;
    };

    struct VcsOperationResult
    {
        VcsError error{VcsError::None};
        // Human-readable detail on failure, e.g. git's stderr output.
        std::string details;
    };

    /**
     * @brief Asynchronous operations against a single Git repository root
     * (#171).
     *
     * All operations deliver their result via callback rather than a return
     * value or exception, so callers never block on the underlying git
     * process.
     */
    class VcsRepositoryInterface
    {
    public:
        virtual ~VcsRepositoryInterface() = default;

        using StatusCallback    = std::function<void(VcsStatusResult)>;
        using OperationCallback = std::function<void(VcsOperationResult)>;

        virtual void status(StatusCallback callback) = 0;

        virtual void stage(const std::vector<std::string>& paths,
                           OperationCallback callback) = 0;

        virtual void unstage(const std::vector<std::string>& paths,
                             OperationCallback callback) = 0;

        virtual void commit(const std::string& message,
                            OperationCallback callback) = 0;

        [[nodiscard]] virtual const std::string& rootPath() const = 0;
    };

    using VcsRepositoryInterfacePtr = std::shared_ptr<VcsRepositoryInterface>;
} // namespace aide

#endif // AIDE_VCS_REPOSITORY_INTERFACE_HPP
