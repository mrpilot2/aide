#ifndef AIDE_APPLICATION_CLOSE_HPP
#define AIDE_APPLICATION_CLOSE_HPP

#include "applicationclosecontroller.hpp"
#include "applicationcloseview.hpp"

namespace aide
{
    class SettingsInterface;
} // namespace aide

namespace aide::core
{
    class ApplicationClose : public ApplicationCloseController
    {
    public:
        ApplicationClose(ApplicationCloseViewWeakPtr v,
                         SettingsInterface& settings);

        [[nodiscard]] bool isCloseAllowed() const override;

    private:
        ApplicationCloseViewWeakPtr view;

        SettingsInterface& settings;
    };
} // namespace aide::core

#endif // AIDE_APPLICATION_CLOSE_HPP
