#ifndef AIDE_DEMO_SETTINGS_PAGE_HPP
#define AIDE_DEMO_SETTINGS_PAGE_HPP

#include <aide/hierarchicalid.hpp>
#include <aide/settings/settingspage.hpp>

class QCheckBox;
class QFormLayout;
class QLabel;

namespace Ui
{
    class DemoSettingsPage;
} // namespace Ui

class DemoSettingsPage : public aide::core::SettingsPage
{
public:
    DemoSettingsPage(aide::HierarchicalId group);

    ~DemoSettingsPage() override;

    [[nodiscard]] QWidget* widget() override;

private:
    void createWidget();

    std::unique_ptr<Ui::DemoSettingsPage> m_ui;

    QWidget* m_widget;
};

#endif // AIDE_DEMO_SETTINGS_PAGE_HPP
