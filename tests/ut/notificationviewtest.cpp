#include <catch2/catch_test_macros.hpp>

#include <QApplication>
#include <QLabel>
#include <QListView>
#include <QPushButton>
#include <QToolButton>

#include <aide/gui/widgets/notificationview.hpp>
#include <aide/hierarchicalid.hpp>
#include <aide/notificationmanager.hpp>

#include "mocksettings.hpp"

using aide::HierarchicalId;
using aide::Notification;
using aide::NotificationManager;
using aide::NotificationType;
using aide::test::MockSettings;
using aide::widgets::NotificationView;

namespace
{
    Notification makeNotification(HierarchicalId groupId)
    {
        Notification notification;
        notification.groupId = std::move(groupId);
        notification.type    = NotificationType::Information;
        notification.title   = "Title";
        notification.content = "Content";
        return notification;
    }

    QLabel* emptyLabel(const NotificationView& view)
    {
        for (auto* label : view.findChildren<QLabel*>()) {
            if (label->text() == QObject::tr("No notifications yet")) {
                return label;
            }
        }
        return nullptr;
    }

    QPushButton* clearAllButton(const NotificationView& view)
    {
        for (auto* button : view.findChildren<QPushButton*>()) {
            if (button->text() == QObject::tr("Clear all")) { return button; }
        }
        return nullptr;
    }

    QToolButton* collapseButton(const NotificationView& view)
    {
        for (auto* button : view.findChildren<QToolButton*>()) {
            if (button->text() == QStringLiteral("—")) { return button; }
        }
        return nullptr;
    }
} // namespace

TEST_CASE("An empty NotificationView", "[NotificationView]")
{
    MockSettings settings;
    NotificationManager manager{settings};

    NotificationView view{manager, settings};
    view.show();
    QApplication::processEvents();

    SECTION("shows the empty state")
    {
        auto* label = emptyLabel(view);
        REQUIRE(label != nullptr);
        REQUIRE(label->isVisible());

        auto* listView = view.findChild<QListView*>();
        REQUIRE(listView != nullptr);
        REQUIRE_FALSE(listView->isVisible());
    }
}

TEST_CASE("A NotificationView with entries", "[NotificationView]")
{
    MockSettings settings;
    NotificationManager manager{settings};
    const HierarchicalId groupId{"build"};
    manager.post(makeNotification(groupId));

    NotificationView view{manager, settings};
    view.show();
    QApplication::processEvents();

    SECTION("shows the list instead of the empty state")
    {
        auto* listView = view.findChild<QListView*>();
        REQUIRE(listView != nullptr);
        REQUIRE(listView->isVisible());
        REQUIRE(listView->model()->rowCount() == 1);

        auto* label = emptyLabel(view);
        REQUIRE(label != nullptr);
        REQUIRE_FALSE(label->isVisible());
    }

    SECTION("Clear all purges the manager's log")
    {
        auto* button = clearAllButton(view);
        REQUIRE(button != nullptr);

        button->click();

        REQUIRE(manager.notifications().empty());
    }
}

TEST_CASE("NotificationView read state", "[NotificationView]")
{
    MockSettings settings;
    NotificationManager manager{settings};
    const HierarchicalId groupId{"build"};
    manager.post(makeNotification(groupId));
    REQUIRE(manager.hasUnread());

    NotificationView view{manager, settings};

    SECTION("not shown yet keeps hasUnread")
    {
        REQUIRE(manager.hasUnread());
    }

    SECTION("becoming visible clears hasUnread")
    {
        view.show();
        QApplication::processEvents();

        REQUIRE_FALSE(manager.hasUnread());
    }
}

TEST_CASE("NotificationView collapse control", "[NotificationView]")
{
    MockSettings settings;
    NotificationManager manager{settings};

    NotificationView view{manager, settings};
    view.show();
    QApplication::processEvents();

    SECTION("toggles the body's visibility")
    {
        auto* collapse = collapseButton(view);
        REQUIRE(collapse != nullptr);

        auto* listView = view.findChild<QListView*>();
        REQUIRE(listView != nullptr);
        auto* body = listView->parentWidget();
        REQUIRE(body != nullptr);
        REQUIRE(body->isVisible());

        collapse->click();

        REQUIRE_FALSE(body->isVisible());
    }
}
