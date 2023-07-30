
#include "aidesettingsprovider.hpp"

#include <utility>

#include "aide/settingsinterface.hpp"
#include "qtsettings.hpp"

using aide::AideSettingsProvider;
using aide::SettingsInterface;

// NOLINTNEXTLINE
std::shared_ptr<SettingsInterface> AideSettingsProvider::m_versionableSettings{
    nullptr};
// NOLINTNEXTLINE
std::shared_ptr<SettingsInterface>
    AideSettingsProvider::m_unversionableSettings{nullptr};

void AideSettingsProvider::provideVersionableSettings(
    std::shared_ptr<SettingsInterface> settings)
{
    m_versionableSettings = std::move(settings);
}

void AideSettingsProvider::provideUnVersionableSettings(
    std::shared_ptr<SettingsInterface> settings)
{
    m_unversionableSettings = std::move(settings);
}

std::shared_ptr<SettingsInterface> AideSettingsProvider::versionableSettings()
{
    return m_versionableSettings;
}

std::shared_ptr<SettingsInterface> AideSettingsProvider::unversionableSettings()
{
    return m_unversionableSettings;
}
