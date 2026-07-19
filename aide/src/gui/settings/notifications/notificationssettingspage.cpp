#include "notificationssettingspage.hpp"

#include <QCheckBox>
#include <QComboBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QVBoxLayout>

#include <aide/gui/widgets/aidetableview.hpp>
#include <aide/hierarchicalid.hpp>
#include <aide/notificationballoonplacement.hpp>
#include <aide/notificationmanagerinterface.hpp>
#include <aide/settingsinterface.hpp>

#include "settings/notifications/notificationgrouptablemodel.hpp"
#include "settings/notifications/notificationpopuptypedelegate.hpp"
#include "settings/notifications/notificationsettingskeys.hpp"
#include "settings/searchpattern.hpp"

using aide::HierarchicalId;
using aide::NotificationDisplayType;
using aide::NotificationManagerInterface;
using aide::SettingsInterface;
using aide::core::notificationBalloonPlacementKey;
using aide::core::notificationDisplayTypeKey;
using aide::core::NotificationGroupTableModel;
using aide::core::tokenizeSearchPattern;
using aide::gui::NotificationPopupTypeDelegate;
using aide::gui::NotificationsSettingsPage;
using aide::widgets::AideTableView;

namespace
{
    constexpr int NO_MATCH_INDEX = -1;
    constexpr int FALLBACK_INDEX = 0;
} // namespace

NotificationsSettingsPage::NotificationsSettingsPage(
    NotificationManagerInterface& manager, SettingsInterface& settings,
    QWidget* parent)
    : QWidget(parent)
    , core::SettingsPage(
          HierarchicalId("Appearance & Behavior")("Notifications"))
    , m_manager(manager)
    , m_settings(settings)
    , m_doNotDisturbCheckBox(new QCheckBox(tr("Do Not Disturb"), this))
    , m_placementCombo(new QComboBox(this))
    , m_tableView(new AideTableView(this))
    , m_model(new NotificationGroupTableModel(this))
{
    m_placementCombo->addItem(
        tr("Bottom right"),
        static_cast<int>(NotificationBalloonPlacement::BottomRight));
    m_placementCombo->addItem(
        tr("Bottom left"),
        static_cast<int>(NotificationBalloonPlacement::BottomLeft));
    m_placementCombo->addItem(
        tr("Top right"),
        static_cast<int>(NotificationBalloonPlacement::TopRight));
    m_placementCombo->addItem(
        tr("Top left"),
        static_cast<int>(NotificationBalloonPlacement::TopLeft));

    m_tableView->setModel(m_model);
    m_tableView->setItemDelegateForColumn(
        NotificationGroupTableModel::PopupTypeColumn,
        new NotificationPopupTypeDelegate(m_tableView));
    m_tableView->horizontalHeader()->setSectionResizeMode(
        NotificationGroupTableModel::GroupColumn, QHeaderView::Stretch);
    m_tableView->verticalHeader()->setVisible(false);

    auto* topRow = new QHBoxLayout;
    topRow->addWidget(m_doNotDisturbCheckBox);
    topRow->addStretch();
    topRow->addWidget(new QLabel(tr("Balloon placement:"), this));
    topRow->addWidget(m_placementCombo);

    auto* layout = new QVBoxLayout(this);
    layout->addLayout(topRow);
    layout->addWidget(m_tableView);
    setLayout(layout);

    syncControlsFromSettings();
}

QWidget* NotificationsSettingsPage::widget()
{
    return this;
}

QStringList NotificationsSettingsPage::groupTitles() const
{
    return {tr("Appearance & Behavior"), tr("Notifications")};
}

bool NotificationsSettingsPage::isModified() const
{
    return m_doNotDisturbCheckBox->isChecked() != m_appliedDoNotDisturb ||
           m_placementCombo->currentIndex() != m_appliedPlacementIndex ||
           currentDisplayTypes() != m_appliedDisplayTypes;
}

void NotificationsSettingsPage::reset()
{
    syncControlsFromSettings();
}

void NotificationsSettingsPage::apply()
{
    m_manager.setDoNotDisturb(m_doNotDisturbCheckBox->isChecked());
    m_settings.setValue(notificationBalloonPlacementKey(),
                        m_placementCombo->currentData());

    for (int row = 0; row < m_model->rowCount(); ++row) {
        const auto& group = m_model->groupAt(row);
        m_settings.setValue(notificationDisplayTypeKey(group.id),
                            static_cast<int>(m_model->displayTypeAt(row)));
    }

    m_appliedDoNotDisturb   = m_doNotDisturbCheckBox->isChecked();
    m_appliedPlacementIndex = m_placementCombo->currentIndex();
    m_appliedDisplayTypes   = currentDisplayTypes();
}

bool NotificationsSettingsPage::matches(const QString& pattern)
{
    if (SettingsPage::matches(pattern)) { return true; }
    return m_model->matchesAnyRow(tokenizeSearchPattern(pattern));
}

double NotificationsSettingsPage::score(const QStringList& words)
{
    return SettingsPage::score(words) + m_model->rowsScore(words);
}

void NotificationsSettingsPage::highlight(const QString& pattern)
{
    SettingsPage::highlight(pattern);
    m_model->setSearchPattern(pattern);
}

void NotificationsSettingsPage::syncControlsFromSettings()
{
    m_doNotDisturbCheckBox->setChecked(m_manager.doNotDisturb());

    const auto placementValue =
        m_settings
            .value(notificationBalloonPlacementKey(),
                   static_cast<int>(NotificationBalloonPlacement::BottomRight))
            .toInt();
    const auto placementIndex = m_placementCombo->findData(placementValue);
    m_placementCombo->setCurrentIndex(
        placementIndex != NO_MATCH_INDEX ? placementIndex : FALLBACK_INDEX);

    m_model->setGroups(m_manager.groups());
    for (int row = 0; row < m_model->rowCount(); ++row) {
        const auto& group = m_model->groupAt(row);
        const auto overrideValue =
            m_settings.value(notificationDisplayTypeKey(group.id));
        if (overrideValue.isValid()) {
            m_model->setDisplayTypeAt(row, static_cast<NotificationDisplayType>(
                                               overrideValue.toInt()));
        }
        m_tableView->openPersistentEditor(
            m_model->index(row, NotificationGroupTableModel::PopupTypeColumn));
    }

    m_appliedDoNotDisturb   = m_doNotDisturbCheckBox->isChecked();
    m_appliedPlacementIndex = m_placementCombo->currentIndex();
    m_appliedDisplayTypes   = currentDisplayTypes();
}

std::vector<NotificationDisplayType>
NotificationsSettingsPage::currentDisplayTypes() const
{
    std::vector<NotificationDisplayType> types;
    types.reserve(static_cast<size_t>(m_model->rowCount()));
    for (int row = 0; row < m_model->rowCount(); ++row) {
        types.push_back(m_model->displayTypeAt(row));
    }
    return types;
}
