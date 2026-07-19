#include "notificationlauncherdialog.hpp"

#include <utility>

#include <QApplication>
#include <QDialogButtonBox>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QPushButton>
#include <QString>
#include <QVBoxLayout>

#include <aide/notification.hpp>
#include <aide/notificationaction.hpp>
#include <aide/notificationmanagerinterface.hpp>

using aide::HierarchicalId;
using aide::Notification;
using aide::NotificationAction;
using aide::NotificationManagerInterface;
using aide::NotificationType;
using demo::NotificationLauncherDialog;

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

    auto* infoButton    = new QPushButton(tr("Info"), group);
    auto* successButton = new QPushButton(tr("Success"), group);
    auto* warningButton = new QPushButton(tr("Warning"), group);
    auto* errorButton   = new QPushButton(tr("Error"), group);

    connect(infoButton, &QPushButton::clicked, this, [this]() {
        postBalloon(NotificationType::Information, tr("Information"),
                    tr("This is an informational balloon notification."));
    });
    connect(successButton, &QPushButton::clicked, this, [this]() {
        postBalloon(NotificationType::Success, tr("Success"),
                    tr("This is a success balloon notification."));
    });
    connect(warningButton, &QPushButton::clicked, this, [this]() {
        postBalloon(NotificationType::Warning, tr("Warning"),
                    tr("This is a warning balloon notification."));
    });
    connect(errorButton, &QPushButton::clicked, this, [this]() {
        postBalloon(NotificationType::Error, tr("Error"),
                    tr("This is an error balloon notification."));
    });

    transientRow->addWidget(infoButton);
    transientRow->addWidget(successButton);
    transientRow->addWidget(warningButton);
    transientRow->addWidget(errorButton);

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
