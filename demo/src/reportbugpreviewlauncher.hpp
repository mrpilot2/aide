#ifndef DEMO_REPORT_BUG_PREVIEW_LAUNCHER_HPP
#define DEMO_REPORT_BUG_PREVIEW_LAUNCHER_HPP

#include <aide/urllauncherinterface.hpp>

namespace demo
{
    // Demo-only aide::UrlLauncherInterface override for the "Report Bug in
    // aIDE" action: shows a preview dialog of the ticket's structure and
    // pre-filled values instead of opening a real browser and filing a real
    // GitHub issue, so routine manual testing of the demo app never
    // accidentally spams the real issue tracker.
    class ReportBugPreviewLauncher : public aide::UrlLauncherInterface
    {
    public:
        bool openUrl(const std::string& url) const override;
    };
} // namespace demo

#endif // DEMO_REPORT_BUG_PREVIEW_LAUNCHER_HPP
