#include "fakevcsprocessrunner.hpp"

using aide::test::FakeVcsProcessRunner;

void FakeVcsProcessRunner::run(const std::string& workingDirectory,
                               const std::string& executable,
                               const std::vector<std::string>& arguments,
                               ResultCallback callback)
{
    ++callCount;
    lastWorkingDirectory = workingDirectory;
    lastExecutable       = executable;
    lastArguments        = arguments;

    callback(nextResult);
}
