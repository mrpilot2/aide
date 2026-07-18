
#include "urllauncherresolver.hpp"

#include <memory>

#include <aide/applicationconfig.hpp>

#include "osurllauncher.hpp"

using aide::UrlLauncherPtr;
using aide::core::UrlLauncherResolver;

UrlLauncherPtr UrlLauncherResolver::resolve(const ApplicationConfig& config)
{
    if (auto overrideLauncher = config.urlLauncher();
        overrideLauncher != nullptr) {
        return overrideLauncher;
    }
    return std::make_shared<OsUrlLauncher>();
}
