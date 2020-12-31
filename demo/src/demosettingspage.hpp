#ifndef AIDE_DEMO_SETTINGS_PAGE_HPP
#define AIDE_DEMO_SETTINGS_PAGE_HPP

#include <aide/hierarchicalid.hpp>
#include <aide/settings/settingspage.hpp>

class QCheckBox;
class QFormLayout;
class QLabel;

class DemoSettingsPage : public aide::core::SettingsPage
{
public:
    DemoSettingsPage(aide::HierarchicalId group);

    ~DemoSettingsPage() override;

    [[nodiscard]] QWidget* widget() override;

private:
    void createWidget();

    QWidget* m_widget;

    QLabel* m_label;

    QCheckBox* m_checkbox;

    QFormLayout* m_layout;
};

#endif // AIDE_DEMO_SETTINGS_PAGE_HPP
