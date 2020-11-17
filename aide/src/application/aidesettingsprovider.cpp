
#include "aidesettingsprovider.hpp"

#include "aide/settingsinterface.hpp"
#include "qtsettings.hpp"

using aide::AideSettingsProvider;
using aide::SettingsInterface;

AideSettingsProvider::AideSettingsProvider()
    : m_versionableSettings(std::make_shared<QtSettings>(true))
    , m_unversionableSettings(std::make_shared<QtSettings>(false))
{}

std::shared_ptr<SettingsInterface> AideSettingsProvider::versionableSettings()
    const
{
    return m_versionableSettings;
}

std::shared_ptr<SettingsInterface> AideSettingsProvider::unversionableSettings()
    const
{
    return m_unversionableSettings;
}
