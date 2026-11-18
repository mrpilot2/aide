#include "showhelpusecase.hpp"

#include <algorithm>
#include <set>
#include <utility>

#include <QUrl>

using aide::HelpTopicId;
using aide::core::ShowHelpUseCase;

ShowHelpUseCase::ShowHelpUseCase(ApplicationConfig config,
                                 UrlLauncherPtr launcher, LoggerPtr logger,
                                 const SettingsPageRegistry& registry)
    : m_config(std::move(config))
    , m_launcher(std::move(launcher))
    , m_logger(std::move(logger))
    , m_registry(registry)
{
    if (m_config.isEnabled(ApplicationConfig::Feature::Help) &&
        m_config.helpBaseUrl().isEmpty()) {
        m_logger->info(
            "Help is enabled but no Help Base URL is set, so help is "
            "inactive");
    }
}

bool ShowHelpUseCase::isHelpAvailable(const HelpTopicId& id) const
{
    return isHelpActive() && !id.isEmpty() &&
           !m_config.isHelpTopicDisabled(id) &&
           !m_config.remappedHelpTopic(id).isEmpty();
}

bool ShowHelpUseCase::showHelp(const HelpTopicId& id) const
{
    if (!isHelpAvailable(id)) { return false; }

    QUrl url = baseUrlWithTrailingSlash();
    url.setPath(url.path() + m_config.remappedHelpTopic(id).value() + '/');
    return open(url);
}

bool ShowHelpUseCase::showHelpContents() const
{
    if (!isHelpActive()) { return false; }

    return open(baseUrlWithTrailingSlash());
}

bool ShowHelpUseCase::showContextHelp(const HelpTopicId& resolvedTopic) const
{
    warnOnceIfUndeclared(resolvedTopic);
    if (isHelpAvailable(resolvedTopic)) { return showHelp(resolvedTopic); }
    return showHelpContents();
}

bool ShowHelpUseCase::isHelpActive() const
{
    return isHelpActive(m_config);
}

bool ShowHelpUseCase::isHelpActive(const ApplicationConfig& config)
{
    return config.isEnabled(ApplicationConfig::Feature::Help) &&
           !config.helpBaseUrl().isEmpty();
}

QUrl ShowHelpUseCase::baseUrlWithTrailingSlash() const
{
    QUrl url(m_config.helpBaseUrl());
    if (!url.path().endsWith('/')) { url.setPath(url.path() + '/'); }
    return url;
}

bool ShowHelpUseCase::open(const QUrl& url) const
{
    const auto urlString = url.toString().toStdString();
    if (m_launcher->openUrl(urlString)) { return true; }

    m_logger->warn("Could not open the browser to show help at {}", urlString);
    return false;
}

std::vector<HelpTopicId> ShowHelpUseCase::declaredHelpTopics() const
{
    std::set<HelpTopicId> topics;
    for (const auto& declared : unresolvedDeclaredHelpTopics()) {
        if (m_config.isHelpTopicDisabled(declared)) { continue; }
        if (const auto resolved = m_config.remappedHelpTopic(declared);
            !resolved.isEmpty()) {
            topics.insert(resolved);
        }
    }
    return {topics.begin(), topics.end()};
}

std::vector<HelpTopicId> ShowHelpUseCase::unresolvedDeclaredHelpTopics() const
{
    auto topics = m_config.declaredWidgetHelpTopics();
    for (const auto& page : m_registry.settingsPages()) {
        if (auto topic = page->helpTopicId(); !topic.isEmpty()) {
            topics.push_back(std::move(topic));
        }
    }
    return topics;
}

void ShowHelpUseCase::warnOnceIfUndeclared(const HelpTopicId& id) const
{
    if (id.isEmpty() || m_warnedUndeclaredTopics.contains(id)) { return; }

    const auto declared = unresolvedDeclaredHelpTopics();
    if (std::ranges::find(declared, id) != declared.end()) { return; }

    m_warnedUndeclaredTopics.insert(id);
    m_logger->warn(
        "Context Help resolved to the Help Topic '{}', which was never "
        "declared; declare it with ApplicationConfig::declareHelpTopic() so "
        "declaredHelpTopics() lists it",
        id.value().toStdString());
}
