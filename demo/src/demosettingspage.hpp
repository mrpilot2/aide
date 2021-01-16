#ifndef AIDE_DEMO_SETTINGS_PAGE_HPP
#define AIDE_DEMO_SETTINGS_PAGE_HPP

#include <QTimer>

#include <aide/hierarchicalid.hpp>
#include <aide/settings/settingspage.hpp>

namespace aide
{
    class SettingsInterface;
}

namespace Ui
{
    class DemoSettingsPage;
} // namespace Ui

class DemoSettingsPage
    : public QObject
    , public aide::core::SettingsPage
{
    Q_OBJECT
public:
    DemoSettingsPage(aide::HierarchicalId group,
                     std::shared_ptr<aide::SettingsInterface> settings);

    ~DemoSettingsPage();

    [[nodiscard]] QWidget* widget() override;

    bool isModified() const override;

    void reset() override;

    void apply() override;

private slots:

    void insertIntoTableWidget();

private:
    void createWidget();

    std::unique_ptr<Ui::DemoSettingsPage> m_ui;

    std::shared_ptr<aide::SettingsInterface> m_settings;

    std::unique_ptr<QTimer> m_tableInsertTimer;
    QWidget* m_widget;
};

#endif // AIDE_DEMO_SETTINGS_PAGE_HPP
