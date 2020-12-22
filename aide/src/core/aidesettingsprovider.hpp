
#ifndef AIDE_AIDE_SETTINGS_PROVIDER_HPP
#define AIDE_AIDE_SETTINGS_PROVIDER_HPP

#include <aide/settingsproviderinterface.hpp>

namespace aide
{
    class SettingsInterface;

    class AideSettingsProvider : public SettingsProviderInterface
    {
    public:
        AideSettingsProvider();

        [[nodiscard]] std::shared_ptr<SettingsInterface> versionableSettings()
            const override;
        [[nodiscard]] std::shared_ptr<SettingsInterface> unversionableSettings()
            const override;

    private:
        std::shared_ptr<SettingsInterface> m_versionableSettings;

        std::shared_ptr<SettingsInterface> m_unversionableSettings;
    };
} // namespace aide

#endif // AIDE_AIDE_SETTINGS_PROVIDER_HPP
