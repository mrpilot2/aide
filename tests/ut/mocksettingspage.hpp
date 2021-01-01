#ifndef AIDE_MOCK_SETTINGS_PAGE_HPP
#define AIDE_MOCK_SETTINGS_PAGE_HPP

#include <aide/settings/settingspage.hpp>

namespace aide
{
    class HierarchicalId;
} // namespace aide

namespace aide::test
{
    class MockSettingsPage : public aide::core::SettingsPage
    {
    public:
        MockSettingsPage();
        MockSettingsPage(const HierarchicalId& group);

        ~MockSettingsPage() override = default;

        QWidget* widget() override;

        void simulateModified(bool modified);

        [[nodiscard]] bool isModified() const override;

    private:
        std::shared_ptr<QWidget> mockWidget;

        bool pageIsModified{false};
    };
} // namespace aide::test

#endif // AIDE_MOCK_SETTINGS_PAGE_HPP
