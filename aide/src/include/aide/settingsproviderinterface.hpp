#ifndef AIDE_SETTINGS_PROVIDER_HPP
#define AIDE_SETTINGS_PROVIDER_HPP

namespace aide
{
    class SettingsInterface;

    class SettingsProviderInterface
    {
    public:
        virtual ~SettingsProviderInterface();

        [[nodiscard]] virtual SettingsInterface& versionableSettings()
            const = 0;

        [[nodiscard]] virtual SettingsInterface& unversionableSettings()
            const = 0;
    };
} // namespace aide

#endif // AIDE_SETTINGS_PROVIDER_HPP
