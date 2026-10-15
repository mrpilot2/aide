#include "aide/applicationconfig.hpp"

#include <map>
#include <set>
#include <utility>

using aide::ApplicationConfig;

class ApplicationConfig::Impl
{
public:
    [[nodiscard]] bool isEnabled(Feature feature) const
    {
        if (const auto it = m_overrides.find(feature);
            it != m_overrides.end()) {
            return it->second;
        }
        return defaultFor(feature);
    }

    void setEnabled(Feature feature, bool enabled)
    {
        m_overrides[feature] = enabled;
    }

    void setUrlLauncher(UrlLauncherPtr launcher)
    {
        m_urlLauncher = std::move(launcher);
    }

    [[nodiscard]] UrlLauncherPtr urlLauncher() const { return m_urlLauncher; }

    void setHelpBaseUrl(const QUrl& baseUrl) { m_helpBaseUrl = baseUrl; }

    [[nodiscard]] QUrl helpBaseUrl() const { return m_helpBaseUrl; }

    void remapHelpTopic(const HelpTopicId& source, const HelpTopicId& target)
    {
        m_helpTopicRemaps[source] = target;
    }

    [[nodiscard]] HelpTopicId remappedHelpTopic(const HelpTopicId& id) const
    {
        if (const auto it = m_helpTopicRemaps.find(id);
            it != m_helpTopicRemaps.end()) {
            return it->second;
        }
        return id;
    }

    void disableHelpTopic(const HelpTopicId& id)
    {
        m_disabledHelpTopics.insert(id);
    }

    [[nodiscard]] bool isHelpTopicDisabled(const HelpTopicId& id) const
    {
        return m_disabledHelpTopics.contains(id);
    }

private:
    // Single source of truth for per-feature defaults. Every feature ships
    // enabled today; flip an entry here to make a future feature opt-in
    // (default-off) without changing the public semantics.
    static bool defaultFor(Feature feature)
    {
        switch (feature) {
        case Feature::ViewFullscreenAction:
        case Feature::Help:
            return true;
        case Feature::ShowLogInFileManagerAction:
        case Feature::ReportBugAction:
            return false;
        }
        return true;
    }

    std::map<Feature, bool> m_overrides;
    UrlLauncherPtr m_urlLauncher;
    QUrl m_helpBaseUrl;
    std::map<HelpTopicId, HelpTopicId> m_helpTopicRemaps;
    std::set<HelpTopicId> m_disabledHelpTopics;
};

ApplicationConfig::ApplicationConfig()
    : m_impl(std::make_unique<Impl>())
{}

ApplicationConfig::~ApplicationConfig() = default;

ApplicationConfig::ApplicationConfig(const ApplicationConfig& other)
    : m_impl(std::make_unique<Impl>(*other.m_impl))
{}

ApplicationConfig& ApplicationConfig::operator=(const ApplicationConfig& other)
{
    if (this != &other) { m_impl = std::make_unique<Impl>(*other.m_impl); }
    return *this;
}

ApplicationConfig::ApplicationConfig(ApplicationConfig&& other) noexcept =
    default;

ApplicationConfig& ApplicationConfig::operator=(
    ApplicationConfig&& other) noexcept = default;

ApplicationConfig& ApplicationConfig::setEnabled(Feature feature, bool enabled)
{
    m_impl->setEnabled(feature, enabled);
    return *this;
}

bool ApplicationConfig::isEnabled(Feature feature) const
{
    return m_impl->isEnabled(feature);
}

ApplicationConfig& ApplicationConfig::setUrlLauncher(UrlLauncherPtr launcher)
{
    m_impl->setUrlLauncher(std::move(launcher));
    return *this;
}

aide::UrlLauncherPtr ApplicationConfig::urlLauncher() const
{
    return m_impl->urlLauncher();
}

ApplicationConfig& ApplicationConfig::setHelpBaseUrl(const QUrl& baseUrl)
{
    m_impl->setHelpBaseUrl(baseUrl);
    return *this;
}

QUrl ApplicationConfig::helpBaseUrl() const
{
    return m_impl->helpBaseUrl();
}

ApplicationConfig& ApplicationConfig::remapHelpTopic(const HelpTopicId& source,
                                                     const HelpTopicId& target)
{
    m_impl->remapHelpTopic(source, target);
    return *this;
}

aide::HelpTopicId ApplicationConfig::remappedHelpTopic(
    const HelpTopicId& id) const
{
    return m_impl->remappedHelpTopic(id);
}

ApplicationConfig& ApplicationConfig::disableHelpTopic(const HelpTopicId& id)
{
    m_impl->disableHelpTopic(id);
    return *this;
}

bool ApplicationConfig::isHelpTopicDisabled(const HelpTopicId& id) const
{
    return m_impl->isHelpTopicDisabled(id);
}
