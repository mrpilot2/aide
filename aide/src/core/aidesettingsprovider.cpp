
#include "aidesettingsprovider.hpp"

#include <utility>

#include "aide/settingsinterface.hpp"
#include "qtsettings.hpp"

using aide::AideSettingsProvider;
using aide::SettingsInterface;

std::shared_ptr<SettingsInterface> AideSettingsProvider::m_versionableSettings{
    std::make_shared<aide::QtSettings>(true)};

std::shared_ptr<SettingsInterface>
    AideSettingsProvider::m_unversionableSettings{
        std::make_shared<aide::QtSettings>(false)};

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
