#ifndef AIDE_FAKE_VCS_PROCESS_RUNNER_HPP
#define AIDE_FAKE_VCS_PROCESS_RUNNER_HPP

#include <string>
#include <vector>

#include <aide/vcsprocessrunnerinterface.hpp>

namespace aide::test
{
    // Test double for VcsProcessRunnerInterface: never spawns a real
    // process. run() records the invocation and immediately invokes
    // callback with nextResult, so tests can drive VcsRepository's command
    // construction and output parsing against canned stdout/stderr/exit
    // codes.
    class FakeVcsProcessRunner : public VcsProcessRunnerInterface
    {
    public:
        void run(const std::string& workingDirectory,
                 const std::string& executable,
                 const std::vector<std::string>& arguments,
                 ResultCallback callback) override;

        VcsProcessResult nextResult;

        int callCount{0};
        std::string lastWorkingDirectory;
        std::string lastExecutable;
        std::vector<std::string> lastArguments;
    };
} // namespace aide::test

#endif // AIDE_FAKE_VCS_PROCESS_RUNNER_HPP
