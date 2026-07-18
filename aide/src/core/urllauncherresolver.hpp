
#ifndef AIDE_URL_LAUNCHER_RESOLVER_HPP
#define AIDE_URL_LAUNCHER_RESOLVER_HPP

#include <aide/urllauncherinterface.hpp>

namespace aide
{
    class ApplicationConfig;
} // namespace aide

namespace aide::core
{
    // Picks the UrlLauncherInterface implementation the "Report Bug in
    // aIDE" action (and any future URL-opening feature) should use: the
    // consumer's ApplicationConfig override if one was supplied, otherwise
    // the default OS-backed launcher.
    class UrlLauncherResolver
    {
    public:
        static UrlLauncherPtr resolve(const ApplicationConfig& config);
    };
} // namespace aide::core

#endif // AIDE_URL_LAUNCHER_RESOLVER_HPP
