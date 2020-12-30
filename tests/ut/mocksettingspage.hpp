#ifndef AIDE_MOCK_SETTINGS_PAGE_HPP
#define AIDE_MOCK_SETTINGS_PAGE_HPP

#include <aide/settings/settingspage.hpp>

namespace aide::test
{
    class MockSettingsPage : public aide::core::SettingsPage
    {
    public:
        MockSettingsPage();
        ~MockSettingsPage() override = default;
    };
} // namespace aide::test

#endif // AIDE_MOCK_SETTINGS_PAGE_HPP
