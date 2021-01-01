#ifndef AIDE_DEMO_SETTINGS_PAGE_HPP
#define AIDE_DEMO_SETTINGS_PAGE_HPP

#include <QTimer>

#include <aide/hierarchicalid.hpp>
#include <aide/settings/settingspage.hpp>

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
    DemoSettingsPage(aide::HierarchicalId group);

    ~DemoSettingsPage();

    [[nodiscard]] QWidget* widget() override;

    bool isModified() const override;

private slots:

    void insertIntoTableWidget();

private:
    void createWidget();

    std::unique_ptr<Ui::DemoSettingsPage> m_ui;

    std::unique_ptr<QTimer> m_tableInsertTimer;
    QWidget* m_widget;
};

#endif // AIDE_DEMO_SETTINGS_PAGE_HPP
