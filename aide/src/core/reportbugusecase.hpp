
#ifndef AIDE_REPORT_BUG_USE_CASE_HPP
#define AIDE_REPORT_BUG_USE_CASE_HPP

#include <aide/loggerinterface.hpp>
#include <aide/urllauncherinterface.hpp>

namespace aide::core
{
    // Builds a pre-filled "new issue" URL against aIDE's own GitHub repo
    // (using the bug.yml issue-form template) and asks the launcher to open
    // it in the system's default browser. Reporter-authored fields (contact,
    // description, reproduction steps, screenshot) are left blank; only the
    // fields aIDE can answer on the reporter's behalf (version and a
    // diagnostic block, which includes the OS) are pre-filled. The log
    // field is deliberately left as a placeholder rather than a path or
    // contents: a log file's location - and anything already in it - may
    // hold data the reporter did not intend to share.
    //
    // The bug.yml template's "Operating System" field is a dropdown, and
    // GitHub issue forms only support pre-filling text/textarea fields via
    // URL query parameters, not dropdowns - so it is not set here; the OS is
    // still visible to the reporter in the diagnostic block.
    class ReportBugUseCase
    {
    public:
        ReportBugUseCase(UrlLauncherPtr launcher, LoggerPtr logger);

        void reportBug() const;

    private:
        UrlLauncherPtr m_launcher;
        LoggerPtr m_logger;
    };
} // namespace aide::core

#endif // AIDE_REPORT_BUG_USE_CASE_HPP
