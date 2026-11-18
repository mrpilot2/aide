#ifndef DEMO_DESKTOP_URL_LAUNCHER_HPP
#define DEMO_DESKTOP_URL_LAUNCHER_HPP

#include <string>

#include <aide/urllauncherinterface.hpp>

namespace demo
{
    // Opens URLs in the system's default browser.
    class DesktopUrlLauncher : public aide::UrlLauncherInterface
    {
    public:
        bool openUrl(const std::string& url) const override;
    };
} // namespace demo

#endif // DEMO_DESKTOP_URL_LAUNCHER_HPP
