#ifndef AIDE_MOCK_SETTINGS_PAGE_HPP
#define AIDE_MOCK_SETTINGS_PAGE_HPP

#include <optional>

#include <aide/settings/settingspage.hpp>

class QString;

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
        explicit MockSettingsPage(const HierarchicalId& group);
        MockSettingsPage(const HierarchicalId& group,
                         const QString& searchableText);

        ~MockSettingsPage() override;

        QWidget* widget() override;

        void simulateModified(bool modified);

        [[nodiscard]] bool isModified() const override;

        void setGroupTitles(QStringList titles);

        [[nodiscard]] QStringList groupTitles() const override;

        void reset() override;

        [[nodiscard]] bool wasResetCalled() const;

        void clearResetWasCalled();

        [[nodiscard]] uint16_t numberOfTimesResetWasCalled() const;

        void apply() override;

        [[nodiscard]] bool wasApplyCalled() const;

    private:
        std::shared_ptr<QWidget> mockWidget;

        bool pageIsModified{false};

        bool applyWasCalled{false};

        bool resetWasCalled{false};

        uint16_t resetCounter{0};

        std::optional<QStringList> overriddenGroupTitles;
    };
} // namespace aide::test

#endif // AIDE_MOCK_SETTINGS_PAGE_HPP
