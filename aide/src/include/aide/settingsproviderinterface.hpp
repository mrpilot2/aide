#ifndef AIDE_SETTINGS_PROVIDER_HPP
#define AIDE_SETTINGS_PROVIDER_HPP

#include <memory>

namespace aide
{
    class SettingsInterface;

    class SettingsProviderInterface
    {
    public:
        virtual ~SettingsProviderInterface();

        /**
         * use this for settings that can be stored in a version control
         * system. If the implementation of SettingsInterface is file
         * based, this creates a separate file to be stored in version control.
         *
         * The user shall be able to easily transfer application settings
         * onto another computer.
         *
         * A setting that can be stored in version control is for example
         * the selected application language.
         * A setting that should not be stored in version control is for example
         * the window geometry, as it might change with every application exit.
         * @return current implementation of SettingsInterface
         */
        [[nodiscard]] virtual std::shared_ptr<SettingsInterface>
        versionableSettings() const = 0;

        /**
         * use this for settings that should not be stored in a version control
         * system. If the implementation of SettingsInterface is file
         * based, this creates a separate file for settings to be stored
         * in version control.
         *
         * The user shall be able to easily transfer application settings
         * onto another computer.
         *
         * A setting that can be stored in version control is for example
         * the selected application language.
         * A setting that should not be stored in version control is for example
         * the window geometry, as it might change with every application exit.
         * @return current implementation of SettingsInterface
         */
        [[nodiscard]] virtual std::shared_ptr<SettingsInterface>
        unversionableSettings() const = 0;
    };
} // namespace aide

#endif // AIDE_SETTINGS_PROVIDER_HPP
