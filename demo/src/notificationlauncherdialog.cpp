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
#include <aide/gui/widgets/banner.hpp>
#include <aide/gui/widgets/gotittooltip.hpp>
#include <aide/mainwindowinterface.hpp>
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
using aide::core::MainWindowInterface;
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
    MainWindowInterface& mainWindow, HierarchicalId balloonGroupId,
    HierarchicalId stickyBalloonGroupId, QWidget* parent)
    : QDialog(parent)
    , m_notificationManager{notificationManager}
    , m_mainWindow{mainWindow}
    , m_balloonGroupId{std::move(balloonGroupId)}
    , m_stickyBalloonGroupId{std::move(stickyBalloonGroupId)}
{
    setWindowTitle(tr("Notifications"));

    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(createBalloonSection());
    mainLayout->addWidget(createGotItSection());
    mainLayout->addWidget(createDialogBannerSection());
    mainLayout->addWidget(createEditorBannerSection());

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
        {.type        = NotificationType::Information,
         .buttonLabel = tr("Info"),
         .title       = tr("Information"),
         .content     = tr("This is an informational balloon notification.")},
        {.type        = NotificationType::Success,
         .buttonLabel = tr("Success"),
         .title       = tr("Success"),
         .content     = tr("This is a success balloon notification.")},
        {.type        = NotificationType::Warning,
         .buttonLabel = tr("Warning"),
         .title       = tr("Warning"),
         .content     = tr("This is a warning balloon notification.")},
        {.type        = NotificationType::Error,
         .buttonLabel = tr("Error"),
         .title       = tr("Error"),
         .content     = tr("This is an error balloon notification.")},
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
        {.position = GotItPosition::Below, .label = tr("Below")},
        {.position = GotItPosition::Above, .label = tr("Above")},
        {.position = GotItPosition::Right, .label = tr("Right")},
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

QWidget* NotificationLauncherDialog::createEditorBannerSection()
{
    // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
    auto* group       = new QGroupBox(tr("Editor banner"));
    auto* groupLayout = new QVBoxLayout(group);

    auto* severityRow = new QHBoxLayout;

    const std::array<SeverityButton, 4> severityButtons{{
        {.type        = NotificationType::Information,
         .buttonLabel = tr("Info"),
         .title       = tr("Information"),
         .content     = tr("This is an informational editor banner.")},
        {.type        = NotificationType::Success,
         .buttonLabel = tr("Success"),
         .title       = tr("Success"),
         .content     = tr("This is a success editor banner.")},
        {.type        = NotificationType::Warning,
         .buttonLabel = tr("Warning"),
         .title       = tr("Warning"),
         .content     = tr("This is a warning editor banner.")},
        {.type        = NotificationType::Error,
         .buttonLabel = tr("Error"),
         .title       = tr("Error"),
         .content     = tr("This is an error editor banner.")},
    }};

    for (const auto& severityButton : severityButtons) {
        auto* button = new QPushButton(severityButton.buttonLabel, group);
        connect(button, &QPushButton::clicked, this, [this, severityButton]() {
            addEditorBanner(severityButton.type, severityButton.content);
        });
        severityRow->addWidget(button);
    }

    auto* clearAllButton = new QPushButton(tr("Clear all"), group);
    connect(clearAllButton, &QPushButton::clicked, this,
            &NotificationLauncherDialog::clearEditorBanners);

    groupLayout->addLayout(severityRow);
    groupLayout->addWidget(clearAllButton);

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

void NotificationLauncherDialog::addEditorBanner(NotificationType type,
                                                 const QString& message)
{
    m_editorBanners.push_back(m_mainWindow.addBanner(type, message));
}

void NotificationLauncherDialog::clearEditorBanners()
{
    for (auto* banner : m_editorBanners) {
        m_mainWindow.removeBanner(banner);
    }
    m_editorBanners.clear();
}
