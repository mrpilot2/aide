
#ifndef AIDE_OS_URL_LAUNCHER_HPP
#define AIDE_OS_URL_LAUNCHER_HPP

#include <aide/urllauncherinterface.hpp>

namespace aide::core
{
    // Cross-platform implementation: opens the OS's default handler for a
    // URL, e.g. the system's default browser.
    class OsUrlLauncher : public UrlLauncherInterface
    {
    public:
        bool openUrl(const std::string& url) const override;
    };
} // namespace aide::core

#endif // AIDE_OS_URL_LAUNCHER_HPP
