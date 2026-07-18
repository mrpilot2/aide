#ifndef AIDE_MOCK_URL_LAUNCHER_HPP
#define AIDE_MOCK_URL_LAUNCHER_HPP

#include <optional>
#include <string>

#include <aide/urllauncherinterface.hpp>

namespace aide::tests
{
    class MockUrlLauncher : public aide::UrlLauncherInterface
    {
    public:
        bool openUrlReturnValue{true};
        mutable std::optional<std::string> lastRequestedUrl;

        bool openUrl(const std::string& url) const override;
    };
} // namespace aide::tests

#endif // AIDE_MOCK_URL_LAUNCHER_HPP
