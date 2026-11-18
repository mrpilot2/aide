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
    //
    // aIDE opens every URL through the one configured launcher, so any other
    // URL (e.g. a Help Page) is passed on to @p browser unchanged.
    class ReportBugPreviewLauncher : public aide::UrlLauncherInterface
    {
    public:
        explicit ReportBugPreviewLauncher(aide::UrlLauncherPtr browser);

        bool openUrl(const std::string& url) const override;

    private:
        aide::UrlLauncherPtr m_browser;
    };
} // namespace demo

#endif // DEMO_REPORT_BUG_PREVIEW_LAUNCHER_HPP
