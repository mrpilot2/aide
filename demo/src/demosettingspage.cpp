#include "demosettingspage.hpp"

#include <utility>

#include <QCheckBox>
#include <QFormLayout>
#include <QLabel>
#include <QWidget>

#include <aide/hierarchicalid.hpp>

#include "ui_demosettingspage.h"

using aide::HierarchicalId;
using aide::core::SettingsPage;

DemoSettingsPage::DemoSettingsPage(aide::HierarchicalId group)
    : SettingsPage(std::move(group))
    , m_ui{std::make_unique<Ui::DemoSettingsPage>()}
    , m_widget(nullptr)
{}

DemoSettingsPage::~DemoSettingsPage() = default;

QWidget* DemoSettingsPage::widget()
{
    if (m_widget == nullptr) { createWidget(); }
    return m_widget;
}

void DemoSettingsPage::createWidget()
{
    // NOLINTNEXTLINE
    m_widget = new QWidget();

    m_ui->setupUi(m_widget);
}
