
#ifndef AIDE_URL_LAUNCHER_INTERFACE_HPP
#define AIDE_URL_LAUNCHER_INTERFACE_HPP

#include <memory>
#include <string>

namespace aide
{
    // Decouples code that needs to open a URL (e.g. in the system's default
    // browser) from the concrete OS mechanism used to do so.
    class UrlLauncherInterface
    {
    public:
        virtual ~UrlLauncherInterface() = default;

        // Opens the given URL with the OS's default handler. Returns
        // whether the OS reported success.
        virtual bool openUrl(const std::string& url) const = 0;
    };

    using UrlLauncherPtr = std::shared_ptr<UrlLauncherInterface>;
} // namespace aide

#endif // AIDE_URL_LAUNCHER_INTERFACE_HPP
