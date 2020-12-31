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

        void setTreeModel(std::shared_ptr<QAbstractItemModel> model) override;

        void setSelectedPageDisplayName(
            const std::string& displayName) override;

        [[nodiscard]] const std::string& displayName() const;

        void showSelectedPageWidget(QWidget* widget) override;

        void showEmptyPageWidget() override;

        [[nodiscard]] QWidget* currentlyShownWidget() const;

    private:
        bool settingsDialogWasExecuted{false};

        std::string currentDisplayName;

        QWidget* settingsWidget{nullptr};
    };
} // namespace aide::test

#endif // AIDE_MOCK_SETTINGS_DIALOG_HPP
