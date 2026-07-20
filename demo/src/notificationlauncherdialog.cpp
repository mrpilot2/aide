#include "notificationlauncherdialog.hpp"

#include <array>
#include <utility>

#include <QApplication>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QPushButton>
#include <QString>
#include <QVBoxLayout>
#include <QVariant>

#include <aide/aidesettingsprovider.hpp>
#include <aide/gui/widgets/gotittooltip.hpp>
#include <aide/notification.hpp>
#include <aide/notificationaction.hpp>
#include <aide/notificationmanagerinterface.hpp>
#include <aide/settingsinterface.hpp>

#include "notificationdemodialog.hpp"

using aide::AideSettingsProvider;
using aide::HierarchicalId;
using aide::Notification;
using aide::NotificationAction;
using aide::NotificationManagerInterface;
using aide::NotificationType;
using aide::widgets::GotItPosition;
using aide::widgets::GotItTooltip;
using demo::NotificationLauncherDialog;

namespace
{
    struct SeverityButton
    {
        NotificationType type;
        QString buttonLabel;
        QString title;
        QString content;
    };

    // Fixed id for the launcher's own "Got it" sample, so the seen-count
    // gate (settings key "aide/gotit/<id>") persists across demo runs the
    // same way a real onboarding tooltip's id would.
    constexpr auto GOT_IT_DEMO_ID = "demo.launcher.gotit";

    struct GotItPositionOption
    {
        GotItPosition position;
        QString label;
    };
} // namespace

NotificationLauncherDialog::NotificationLauncherDialog(
    NotificationManagerInterface& notificationManager,
    HierarchicalId balloonGroupId, HierarchicalId stickyBalloonGroupId,
    QWidget* parent)
    : QDialog(parent)
    , m_notificationManager{notificationManager}
    , m_balloonGroupId{std::move(balloonGroupId)}
    , m_stickyBalloonGroupId{std::move(stickyBalloonGroupId)}
{
    setWindowTitle(tr("Notifications"));

    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(createBalloonSection());
    mainLayout->addWidget(createGotItSection());
    mainLayout->addWidget(createDialogBannerSection());

    auto* buttonBox = new QDialogButtonBox(QDialogButtonBox::Close, this);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    mainLayout->addWidget(buttonBox);
}

QWidget* NotificationLauncherDialog::createBalloonSection()
{
    // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
    auto* group       = new QGroupBox(tr("Balloon"));
    auto* groupLayout = new QVBoxLayout(group);

    auto* transientRow = new QHBoxLayout;

    const std::array<SeverityButton, 4> severityButtons{{
        {NotificationType::Information, tr("Info"), tr("Information"),
         tr("This is an informational balloon notification.")},
        {NotificationType::Success, tr("Success"), tr("Success"),
         tr("This is a success balloon notification.")},
        {NotificationType::Warning, tr("Warning"), tr("Warning"),
         tr("This is a warning balloon notification.")},
        {NotificationType::Error, tr("Error"), tr("Error"),
         tr("This is an error balloon notification.")},
    }};

    for (const auto& severityButton : severityButtons) {
        auto* button = new QPushButton(severityButton.buttonLabel, group);
        connect(button, &QPushButton::clicked, this, [this, severityButton]() {
            postBalloon(severityButton.type, severityButton.title,
                        severityButton.content);
        });
        transientRow->addWidget(button);
    }

    auto* extraRow = new QHBoxLayout;

    auto* actionsButton = new QPushButton(tr("With 2 actions"), group);
    connect(actionsButton, &QPushButton::clicked, this,
            &NotificationLauncherDialog::postBalloonWithActions);

    auto* stickyButton = new QPushButton(tr("Sticky"), group);
    connect(stickyButton, &QPushButton::clicked, this,
            &NotificationLauncherDialog::postStickyBalloon);

    extraRow->addWidget(actionsButton);
    extraRow->addWidget(stickyButton);

    groupLayout->addLayout(transientRow);
    groupLayout->addLayout(extraRow);

    return group;
}

QWidget* NotificationLauncherDialog::createGotItSection()
{
    // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
    auto* group       = new QGroupBox(tr("Got it"));
    auto* groupLayout = new QVBoxLayout(group);

    auto* anchorRow     = new QHBoxLayout;
    m_gotItAnchorButton = new QPushButton(tr("Sample anchor"), group);
    anchorRow->addWidget(m_gotItAnchorButton);
    anchorRow->addStretch(1);

    m_gotItPositionCombo = new QComboBox(group);
    const std::array<GotItPositionOption, 3> positionOptions{{
        {GotItPosition::Below, tr("Below")},
        {GotItPosition::Above, tr("Above")},
        {GotItPosition::Right, tr("Right")},
    }};
    for (const auto& option : positionOptions) {
        m_gotItPositionCombo->addItem(
            option.label,
            QVariant::fromValue(static_cast<int>(option.position)));
    }

    auto* controlsRow = new QHBoxLayout;
    controlsRow->addWidget(m_gotItPositionCombo);

    auto* showButton = new QPushButton(tr("Show"), group);
    connect(showButton, &QPushButton::clicked, this,
            &NotificationLauncherDialog::showGotIt);

    auto* resetButton = new QPushButton(tr("Reset seen flag"), group);
    connect(resetButton, &QPushButton::clicked, this,
            &NotificationLauncherDialog::resetGotItSeenFlag);

    controlsRow->addWidget(showButton);
    controlsRow->addWidget(resetButton);

    groupLayout->addLayout(anchorRow);
    groupLayout->addLayout(controlsRow);

    return group;
}

QWidget* NotificationLauncherDialog::createDialogBannerSection()
{
    // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
    auto* group       = new QGroupBox(tr("Dialog banner"));
    auto* groupLayout = new QVBoxLayout(group);

    auto* openButton = new QPushButton(tr("Open dialog banner demo"), group);
    connect(openButton, &QPushButton::clicked, this, [this]() {
        demo::NotificationDemoDialog dialog(this);
        dialog.exec();
    });

    groupLayout->addWidget(openButton);

    return group;
}

void NotificationLauncherDialog::showGotIt()
{
    const auto settings = AideSettingsProvider::unversionableSettings();
    if (settings == nullptr) { return; }

    const auto position =
        static_cast<GotItPosition>(m_gotItPositionCombo->currentData().toInt());

    // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
    auto* tooltip = new GotItTooltip(
        *settings, GOT_IT_DEMO_ID,
        tr("This is a sample \"Got it\" tooltip shown by the demo launcher."));
    tooltip->showGotIt(m_gotItAnchorButton, position);
}

void NotificationLauncherDialog::resetGotItSeenFlag()
{
    if (const auto settings = AideSettingsProvider::unversionableSettings();
        settings != nullptr) {
        settings->removeKey(HierarchicalId("aide")("gotit")(GOT_IT_DEMO_ID));
    }
}

void NotificationLauncherDialog::postBalloon(NotificationType type,
                                             const QString& title,
                                             const QString& content)
{
    Notification notification;
    notification.groupId = m_balloonGroupId;
    notification.type    = type;
    notification.title   = title;
    notification.content = content;
    m_notificationManager.post(std::move(notification));
}

void NotificationLauncherDialog::postBalloonWithActions()
{
    Notification notification;
    notification.groupId = m_balloonGroupId;
    notification.type    = NotificationType::Information;
    notification.title   = tr("Action required");
    notification.content = tr("This balloon carries two actions.");
    notification.actions = {
        NotificationAction{.title = tr("Retry"),
                           .handler =
                               [this]() {
                                   QMessageBox::information(
                                       this, tr("Notifications"),
                                       tr("\"Retry\" action triggered."));
                               }},
        NotificationAction{
            .title = tr("View Details"),
            .handler =
                [this]() {
                    QMessageBox::information(
                        this, tr("Notifications"),
                        tr("\"View Details\" action triggered."));
                }},
    };
    m_notificationManager.post(std::move(notification));
}

void NotificationLauncherDialog::postStickyBalloon()
{
    Notification notification;
    notification.groupId = m_stickyBalloonGroupId;
    notification.type    = NotificationType::Information;
    notification.title   = tr("Sticky");
    notification.content =
        tr("This balloon has no auto-dismiss timer; close it manually.");
    m_notificationManager.post(std::move(notification));
}
