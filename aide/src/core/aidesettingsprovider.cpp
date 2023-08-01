
#include "aidesettingsprovider.hpp"

#include <utility>

#include "aide/settingsinterface.hpp"
#include "qtsettings.hpp"

using aide::AideSettingsProvider;
using aide::SettingsInterface;

using SettingsInterfacePtr = std::shared_ptr<SettingsInterface>;

// NOLINTNEXTLINE
SettingsInterfacePtr AideSettingsProvider::m_versionableSettings{nullptr};
// NOLINTNEXTLINE
SettingsInterfacePtr AideSettingsProvider::m_unversionableSettings{nullptr};

void AideSettingsProvider::provideVersionableSettings(
    SettingsInterfacePtr settings)
{
    m_versionableSettings = std::move(settings);
}

void AideSettingsProvider::provideUnVersionableSettings(
    SettingsInterfacePtr settings)
{
    m_unversionableSettings = std::move(settings);
}

SettingsInterfacePtr AideSettingsProvider::versionableSettings()
{
    return m_versionableSettings;
}

SettingsInterfacePtr AideSettingsProvider::unversionableSettings()
{
    return m_unversionableSettings;
}
