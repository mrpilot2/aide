
#include "aidesettingsprovider.hpp"

#include <utility>

#include "aide/settingsinterface.hpp"
#include "qtsettings.hpp"

using aide::AideSettingsProvider;
using aide::SettingsInterface;

std::shared_ptr<SettingsInterface> AideSettingsProvider::m_versionableSettings{
    nullptr};
std::shared_ptr<SettingsInterface>
    AideSettingsProvider::m_unversionableSettings{nullptr};

void AideSettingsProvider::provideVersionableSettings(
    std::shared_ptr<SettingsInterface> settings)
{
    m_versionableSettings = settings;
}

void AideSettingsProvider::provideUnVersionableSettings(
    std::shared_ptr<SettingsInterface> settings)
{
    m_unversionableSettings = settings;
}

std::shared_ptr<SettingsInterface> AideSettingsProvider::versionableSettings()
{
    return m_versionableSettings;
}

std::shared_ptr<SettingsInterface> AideSettingsProvider::unversionableSettings()
{
    return m_unversionableSettings;
}
