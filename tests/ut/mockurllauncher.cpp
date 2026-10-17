#include "mockurllauncher.hpp"

using aide::test::MockUrlLauncher;

bool MockUrlLauncher::openUrl(const std::string& url) const
{
    lastRequestedUrl = url;
    return openUrlReturnValue;
}
