#ifndef AIDE_VCS_PROCESS_RUNNER_INTERFACE_HPP
#define AIDE_VCS_PROCESS_RUNNER_INTERFACE_HPP

#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace aide
{
    /**
     * @brief Outcome of running a single external process to completion.
     */
    struct VcsProcessResult
    {
        int exitCode{-1};
        bool crashed{false};
        std::string standardOutput;
        std::string standardError;
    };

    /**
     * @brief Seam between aide::vcs repository/manager logic and process
     * execution (#171).
     *
     * Lets VcsRepository/VcsManager construct and run a command without
     * knowing whether it is backed by a real QProcess or a fake fed canned
     * output in tests.
     */
    class VcsProcessRunnerInterface
    {
    public:
        virtual ~VcsProcessRunnerInterface() = default;

        using ResultCallback = std::function<void(VcsProcessResult)>;

        // Runs executable with arguments in workingDirectory, delivering the
        // result asynchronously via callback. Never throws; a failure to
        // even start the process (e.g. executable not found) is reported
        // through the callback via VcsProcessResult::crashed.
        virtual void run(const std::string& workingDirectory,
                         const std::string& executable,
                         const std::vector<std::string>& arguments,
                         ResultCallback callback) = 0;
    };

    using VcsProcessRunnerInterfacePtr =
        std::shared_ptr<VcsProcessRunnerInterface>;
} // namespace aide

#endif // AIDE_VCS_PROCESS_RUNNER_INTERFACE_HPP
