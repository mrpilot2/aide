#include "appearancepage.hpp"

#include <QComboBox>
#include <QFontComboBox>
#include <QFormLayout>
#include <QSignalBlocker>
#include <QSpinBox>

#include "aide/appearancemanager.hpp"
#include "aide/hierarchicalid.hpp"

namespace
{
    constexpr int MIN_FONT_SIZE = 6;
    constexpr int MAX_FONT_SIZE = 32;
} // namespace

using aide::gui::AppearancePage;

AppearancePage::AppearancePage(aide::AppearanceManager& manager,
                               QWidget* parent)
    : QWidget(parent)
    , core::SettingsPage(HierarchicalId("Appearance & Behavior")("Appearance"))
    , m_manager(manager)
    , m_themeCombo(new QComboBox(this))
    , m_fontCombo(new QFontComboBox(this))
    , m_fontSizeSpinBox(new QSpinBox(this))
{
    m_fontCombo->setFontFilters(QFontComboBox::ScalableFonts);

    m_fontSizeSpinBox->setRange(MIN_FONT_SIZE, MAX_FONT_SIZE);
    m_fontSizeSpinBox->setSuffix(" pt");

    auto* layout = new QFormLayout(this);
    layout->addRow(tr("Theme:"), m_themeCombo);
    layout->addRow(tr("Font:"), m_fontCombo);
    layout->addRow(tr("Size:"), m_fontSizeSpinBox);
    setLayout(layout);

    syncControlsToManager();
}

QWidget* AppearancePage::widget()
{
    return this;
}

bool AppearancePage::isModified() const
{
    return m_themeCombo->currentText() != m_appliedTheme ||
           m_fontCombo->currentFont().family() != m_appliedFontFamily ||
           m_fontSizeSpinBox->value() != m_appliedFontSize;
}

QStringList AppearancePage::groupTitles() const
{
    return {tr("Appearance & Behavior"), tr("Appearance")};
}

void AppearancePage::syncControlsToManager()
{
    // Repopulate the theme list rather than doing it once in the constructor:
    // consumers may register themes after ApplicationBuilder (and thus this
    // page) is constructed, so refresh whenever the dialog is (re)opened.
    const QSignalBlocker blocker(m_themeCombo);
    m_themeCombo->clear();
    m_themeCombo->addItems(m_manager.themeNames());

    m_themeCombo->setCurrentText(m_manager.activeThemeName());
    m_fontCombo->setCurrentFont(QFont{m_manager.activeFont().family()});
    m_fontSizeSpinBox->setValue(m_manager.activeFont().pointSize());

    // Capture the baseline from the controls rather than the manager: a
    // QFontComboBox snaps setCurrentFont() to the nearest available font, so
    // the manager's family may not match the control's resulting font.
    m_appliedTheme      = m_themeCombo->currentText();
    m_appliedFontFamily = m_fontCombo->currentFont().family();
    m_appliedFontSize   = m_fontSizeSpinBox->value();
}

void AppearancePage::reset()
{
    syncControlsToManager();
}

void AppearancePage::apply()
{
    m_manager.applyAppearance(m_themeCombo->currentText(),
                              m_fontCombo->currentFont().family(),
                              m_fontSizeSpinBox->value());

    m_appliedTheme      = m_themeCombo->currentText();
    m_appliedFontFamily = m_fontCombo->currentFont().family();
    m_appliedFontSize   = m_fontSizeSpinBox->value();
}
