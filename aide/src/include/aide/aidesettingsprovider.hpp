
#ifndef AIDE_AIDE_SETTINGS_PROVIDER_HPP
#define AIDE_AIDE_SETTINGS_PROVIDER_HPP

#include "settingsproviderinterface.hpp"

namespace aide
{
    class SettingsInterface;

    class AideSettingsProvider
    {
    public:
        static void provideVersionableSettings(
            std::shared_ptr<SettingsInterface> settings);

        static void provideUnVersionableSettings(
            std::shared_ptr<SettingsInterface> settings);

        [[nodiscard]] static std::shared_ptr<SettingsInterface>
        versionableSettings();

        [[nodiscard]] static std::shared_ptr<SettingsInterface>
        unversionableSettings();

    private:
        static std::shared_ptr<SettingsInterface> m_versionableSettings;

        static std::shared_ptr<SettingsInterface> m_unversionableSettings;
    };
} // namespace aide

#endif // AIDE_AIDE_SETTINGS_PROVIDER_HPP
