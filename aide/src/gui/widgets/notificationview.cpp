#include <QHBoxLayout>
#include <QLabel>
#include <QListView>
#include <QMenu>
#include <QPushButton>
#include <QToolButton>
#include <QVBoxLayout>

#include <aide/gui/widgets/notificationview.hpp>
#include <aide/notificationmanagerinterface.hpp>
#include <aide/settingsinterface.hpp>

#include "notificationlogitemdelegate.hpp"
#include "notifications/notificationlogmodel.hpp"

using aide::NotificationManagerInterface;
using aide::SettingsInterface;
using aide::core::NotificationLogModel;
using aide::widgets::NotificationLogItemDelegate;
using aide::widgets::NotificationView;

namespace
{
    constexpr int LIST_SPACING{6};
} // namespace

NotificationView::NotificationView(NotificationManagerInterface& manager,
                                   SettingsInterface& settings, QWidget* parent)
    : QWidget(parent)
    , m_manager(manager)
    , m_body(new QWidget(this))
    , m_collapseButton(new QToolButton(this))
    , m_listView(new QListView(m_body))
    , m_emptyLabel(new QLabel(tr("No notifications yet"), m_body))
    , m_model(new NotificationLogModel(manager, this))
    , m_delegate(new NotificationLogItemDelegate(settings, this))
{
    auto* rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);

    auto* headerLayout = new QHBoxLayout;
    auto* titleLabel   = new QLabel(tr("Notifications"), this);
    QFont titleFont    = titleLabel->font();
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);

    auto* overflowButton = new QToolButton(this);
    overflowButton->setText(QStringLiteral("⋮"));
    overflowButton->setAutoRaise(true);
    overflowButton->setPopupMode(QToolButton::InstantPopup);
    auto* overflowMenu = new QMenu(overflowButton);
    connect(overflowMenu->addAction(tr("Notification Settings…")),
            &QAction::triggered, this, &NotificationView::settingsRequested);
    overflowButton->setMenu(overflowMenu);

    m_collapseButton->setText(QStringLiteral("—"));
    m_collapseButton->setAutoRaise(true);
    connect(m_collapseButton, &QToolButton::clicked, this,
            &NotificationView::toggleCollapsed);

    headerLayout->addWidget(titleLabel);
    headerLayout->addStretch(1);
    headerLayout->addWidget(overflowButton);
    headerLayout->addWidget(m_collapseButton);
    rootLayout->addLayout(headerLayout);
    rootLayout->addWidget(m_body);

    auto* bodyLayout = new QVBoxLayout(m_body);
    bodyLayout->setContentsMargins(0, 0, 0, 0);

    auto* timelineLayout = new QHBoxLayout;
    auto* timelineLabel  = new QLabel(tr("Timeline"), m_body);
    auto* clearAllButton = new QPushButton(tr("Clear all"), m_body);
    clearAllButton->setFlat(true);
    connect(clearAllButton, &QPushButton::clicked, this,
            [this]() { m_manager.clearAll(); });

    timelineLayout->addWidget(timelineLabel);
    timelineLayout->addStretch(1);
    timelineLayout->addWidget(clearAllButton);
    bodyLayout->addLayout(timelineLayout);

    m_listView->setModel(m_model);
    m_listView->setItemDelegate(m_delegate);
    m_listView->setResizeMode(QListView::Adjust);
    m_listView->setUniformItemSizes(false);
    m_listView->setSpacing(LIST_SPACING);
    m_listView->setSelectionMode(QAbstractItemView::NoSelection);
    m_listView->setMouseTracking(true);

    m_emptyLabel->setAlignment(Qt::AlignCenter);

    bodyLayout->addWidget(m_listView);
    bodyLayout->addWidget(m_emptyLabel);

    connect(m_delegate, &NotificationLogItemDelegate::groupSettingsRequested,
            this, &NotificationView::groupSettingsRequested);

    connect(m_model, &NotificationLogModel::rowsInserted, this, [this]() {
        updateEmptyState();
        if (isVisible()) { m_manager.markRead(); }
    });
    connect(m_model, &NotificationLogModel::rowsRemoved, this,
            &NotificationView::updateEmptyState);
    connect(m_model, &NotificationLogModel::modelReset, this,
            &NotificationView::updateEmptyState);

    updateEmptyState();
}

void NotificationView::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);
    m_manager.markRead();
}

void NotificationView::updateEmptyState()
{
    const bool empty = m_model->rowCount() == 0;
    m_listView->setVisible(!empty);
    m_emptyLabel->setVisible(empty);
}

void NotificationView::toggleCollapsed()
{
    m_body->setVisible(!m_body->isVisible());
}
