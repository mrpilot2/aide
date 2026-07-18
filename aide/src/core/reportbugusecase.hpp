
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
    // fields aIDE can answer on the reporter's behalf (version, OS, and a
    // diagnostic block plus a pointer to the current session's log file) are
    // pre-filled.
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
