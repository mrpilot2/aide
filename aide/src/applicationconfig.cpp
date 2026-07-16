#include "aide/applicationconfig.hpp"

#include <map>

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

private:
    // Single source of truth for per-feature defaults. Every feature ships
    // enabled today; flip an entry here to make a future feature opt-in
    // (default-off) without changing the public semantics.
    static bool defaultFor(Feature feature)
    {
        switch (feature) {
        case Feature::ViewFullscreenAction:
            return true;
        case Feature::ShowLogInFileManagerAction:
            return false;
        }
        return true;
    }

    std::map<Feature, bool> m_overrides;
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
