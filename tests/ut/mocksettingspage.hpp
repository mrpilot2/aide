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

    private:
        std::shared_ptr<QWidget> mockWidget;
    };
} // namespace aide::test

#endif // AIDE_MOCK_SETTINGS_PAGE_HPP
