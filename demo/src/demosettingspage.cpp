#include "demosettingspage.hpp"

#include <utility>

#include <QCheckBox>
#include <QFormLayout>
#include <QLabel>
#include <QWidget>

#include <aide/hierarchicalid.hpp>

using aide::HierarchicalId;
using aide::core::SettingsPage;

DemoSettingsPage::DemoSettingsPage(aide::HierarchicalId group)
    : SettingsPage(std::move(group))
    , m_widget(nullptr)
    , m_label(nullptr)
    , m_checkbox(nullptr)
    , m_layout(nullptr)
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

    // NOLINTNEXTLINE
    m_label = new QLabel("Test label", m_widget);
    // NOLINTNEXTLINE
    m_checkbox = new QCheckBox("Test checkbox", m_widget);
    // NOLINTNEXTLINE
    m_layout = new QFormLayout(m_widget);

    m_layout->addWidget(m_label);
    m_layout->addWidget(m_checkbox);

    m_widget->setLayout(m_layout);
}
