#ifndef AIDE_MOCK_SETTINGS_DIALOG_HPP
#define AIDE_MOCK_SETTINGS_DIALOG_HPP

#include "settings/settingsdialoginterface.hpp"

namespace aide::test
{
    class MockSettingsDialog : public aide::core::SettingsDialogInterface
    {
    public:
        void executeDialog() override;

        [[nodiscard]] bool wasSettingsDialogExecuted() const;

    private:
        bool settingsDialogWasExecuted{false};
    };
} // namespace aide::test

#endif // AIDE_MOCK_SETTINGS_DIALOG_HPP
