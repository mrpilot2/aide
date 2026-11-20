#ifndef AIDE_SHOW_HELP_USE_CASE_HPP
#define AIDE_SHOW_HELP_USE_CASE_HPP

#include <set>
#include <vector>

#include <aide/applicationconfig.hpp>
#include <aide/helptopicid.hpp>
#include <aide/loggerinterface.hpp>
#include <aide/settings/settingspageregistry.hpp>
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
        // @p registry supplies the settings pages' Help Topics for
        // declaredHelpTopics(); it must outlive the use case.
        ShowHelpUseCase(ApplicationConfig config, UrlLauncherPtr launcher,
                        LoggerPtr logger, const SettingsPageRegistry& registry);

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

        // Context Help for the topic resolved from the focused widget (see
        // gui/contexthelptopic.hpp): opens it when available, else Help
        // Contents. Returns false, launching nothing, when help is inactive.
        // The first time a topic was never declared (see
        // declaredHelpTopics()), a warning is logged once for that topic.
        bool showContextHelp(const HelpTopicId& resolvedTopic) const;

        // Every Help Topic the application resolves to, sorted and
        // de-duplicated: the registered settings pages' topics plus the
        // config's declared widget topics, minus disabled ones (checked
        // against the declared ID), with remaps applied one step.
        // Independent of whether help is active.
        [[nodiscard]] std::vector<HelpTopicId> declaredHelpTopics() const;

    private:
        // Where @p id points after the client's config is applied: empty if
        // @p id is empty or disabled, otherwise its (one-step) remap target.
        [[nodiscard]] HelpTopicId resolvedHelpTopic(
            const HelpTopicId& id) const;

        // The settings pages' and declared widget topics as declared: before
        // disabling or remapping, possibly with duplicates.
        [[nodiscard]] std::vector<HelpTopicId> unresolvedDeclaredHelpTopics()
            const;

        void warnOnceIfUndeclared(const HelpTopicId& id) const;

        [[nodiscard]] QUrl baseUrlWithTrailingSlash() const;

        bool open(const QUrl& url) const;

        ApplicationConfig m_config;
        UrlLauncherPtr m_launcher;
        LoggerPtr m_logger;
        const SettingsPageRegistry& m_registry;
        mutable std::set<HelpTopicId> m_warnedUndeclaredTopics;
    };
} // namespace aide::core

#endif // AIDE_SHOW_HELP_USE_CASE_HPP
