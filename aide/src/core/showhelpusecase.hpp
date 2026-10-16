#ifndef AIDE_SHOW_HELP_USE_CASE_HPP
#define AIDE_SHOW_HELP_USE_CASE_HPP

#include <aide/applicationconfig.hpp>
#include <aide/helptopicid.hpp>
#include <aide/loggerinterface.hpp>
#include <aide/urllauncherinterface.hpp>

namespace aide::core
{
    // Resolves Help Topics against the client's ApplicationConfig and opens
    // the resulting Help Page in the browser.
    //
    // Help is active only when Feature::Help is enabled and a Help Base URL
    // is set. The feature being on without a URL is not an error: help just
    // stays inactive, which is logged once, at construction.
    class ShowHelpUseCase
    {
    public:
        ShowHelpUseCase(ApplicationConfig config, UrlLauncherPtr launcher,
                        LoggerPtr logger);

        // True when Feature::Help is enabled and a Help Base URL is set.
        [[nodiscard]] bool isHelpActive() const;

        // The same rule for a config alone, for callers that hold only the
        // config (MainWindow gates the Help Contents action on it).
        [[nodiscard]] static bool isHelpActive(const ApplicationConfig& config);

        // True when help is active and the topic is non-empty and not
        // disabled. Disabling is checked against the declared ID, before any
        // remap.
        [[nodiscard]] bool isHelpAvailable(const HelpTopicId& id) const;

        // Opens <Help Base URL>/<topic-id>/ (after remapping). Returns false,
        // launching nothing, when the topic is not available.
        bool showHelp(const HelpTopicId& id) const;

        // Opens the Help Base URL root. Returns false, launching nothing,
        // when help is inactive.
        bool showHelpContents() const;

    private:
        [[nodiscard]] QUrl baseUrlWithTrailingSlash() const;

        bool open(const QUrl& url) const;

        ApplicationConfig m_config;
        UrlLauncherPtr m_launcher;
        LoggerPtr m_logger;
    };
} // namespace aide::core

#endif // AIDE_SHOW_HELP_USE_CASE_HPP
