#include "mockurllauncher.hpp"

using aide::tests::MockUrlLauncher;

bool MockUrlLauncher::openUrl(const std::string& url) const
{
    lastRequestedUrl = url;
    return openUrlReturnValue;
}
