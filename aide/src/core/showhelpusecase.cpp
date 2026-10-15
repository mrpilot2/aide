#include "showhelpusecase.hpp"

#include <utility>

#include <QUrl>

using aide::HelpTopicId;
using aide::core::ShowHelpUseCase;

ShowHelpUseCase::ShowHelpUseCase(ApplicationConfig config,
                                 UrlLauncherPtr launcher, LoggerPtr logger)
    : m_config(std::move(config))
    , m_launcher(std::move(launcher))
    , m_logger(std::move(logger))
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

bool ShowHelpUseCase::isHelpActive() const
{
    return m_config.isEnabled(ApplicationConfig::Feature::Help) &&
           !m_config.helpBaseUrl().isEmpty();
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
