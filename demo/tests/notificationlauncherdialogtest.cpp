#include <optional>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include <QGroupBox>
#include <QPushButton>
#include <QTest>

#include <aide/hierarchicalid.hpp>
#include <aide/notification.hpp>
#include <aide/notificationgroup.hpp>
#include <aide/notificationid.hpp>
#include <aide/notificationmanagerinterface.hpp>

#include "notificationlauncherdialog.hpp"

using aide::HierarchicalId;
using aide::Notification;
using aide::NotificationDisplayType;
using aide::NotificationGroup;
using aide::NotificationId;
using aide::NotificationManagerInterface;
using aide::NotificationType;
using demo::NotificationLauncherDialog;

namespace
{
    // Records every post() call so the acceptance criteria for #164 (which
    // buttons post which notification, to which group) can be asserted
    // without a real balloon host or main window.
    class FakeNotificationManager : public NotificationManagerInterface
    {
    public:
        [[nodiscard]] const std::vector<Notification>& posted() const
        {
            return m_posted;
        }

        void registerGroup(NotificationGroup group) override
        {
            m_groups.push_back(std::move(group));
        }

        [[nodiscard]] std::optional<NotificationGroup> group(
            const HierarchicalId& /*id*/) const override
        {
            return std::nullopt;
        }

        [[nodiscard]] std::vector<NotificationGroup> groups() const override
        {
            return m_groups;
        }

        NotificationId post(Notification notification) override
        {
            m_posted.push_back(std::move(notification));
            return NotificationId{m_posted.size()};
        }

        void remove(NotificationId /*id*/) override {}

        void markRead(NotificationId /*id*/) override {}

        void clearAll() override { m_posted.clear(); }

        [[nodiscard]] std::vector<Notification> notifications() const override
        {
            return m_posted;
        }

        [[nodiscard]] std::optional<Notification> notification(
            NotificationId /*id*/) const override
        {
            return std::nullopt;
        }

        [[nodiscard]] bool isRead(NotificationId /*id*/) const override
        {
            return false;
        }

        [[nodiscard]] NotificationDisplayType resolvedDisplayType(
            NotificationId /*id*/) const override
        {
            return NotificationDisplayType::None;
        }

        [[nodiscard]] bool doNotDisturb() const override { return false; }

        void setDoNotDisturb(bool /*enabled*/) override {}

        [[nodiscard]] bool hasUnread() const override { return false; }

        void markRead() override {}

    private:
        std::vector<Notification> m_posted;
        std::vector<NotificationGroup> m_groups;
    };

    QPushButton* findButton(const QWidget& parent, const QString& text)
    {
        for (auto* button : parent.findChildren<QPushButton*>()) {
            if (button->text() == text) { return button; }
        }
        return nullptr;
    }
} // namespace

TEST_CASE("A NotificationLauncherDialog", "[NotificationLauncherDialog]")
{
    FakeNotificationManager manager;
    const HierarchicalId balloonGroupId{HierarchicalId("Demo")("Balloon")};
    const HierarchicalId stickyGroupId{
        HierarchicalId("Demo")("Sticky Balloon")};

    NotificationLauncherDialog dialog(manager, balloonGroupId, stickyGroupId);
    dialog.show();
    [[maybe_unused]] const bool exposed = QTest::qWaitForWindowExposed(&dialog);

    SECTION("has a Balloon group box")
    {
        const auto* group = dialog.findChild<QGroupBox*>();
        REQUIRE(group != nullptr);
        REQUIRE(group->title() == "Balloon");
    }

    SECTION(
        "Info button posts an Information notification to the balloon "
        "group")
    {
        QTest::mouseClick(findButton(dialog, "Info"), Qt::LeftButton);

        REQUIRE(manager.posted().size() == 1);
        CHECK(manager.posted().back().type == NotificationType::Information);
        CHECK(manager.posted().back().groupId == balloonGroupId);
    }

    SECTION(
        "Success button posts a Success notification to the balloon "
        "group")
    {
        QTest::mouseClick(findButton(dialog, "Success"), Qt::LeftButton);

        REQUIRE(manager.posted().size() == 1);
        CHECK(manager.posted().back().type == NotificationType::Success);
        CHECK(manager.posted().back().groupId == balloonGroupId);
    }

    SECTION(
        "Warning button posts a Warning notification to the balloon "
        "group")
    {
        QTest::mouseClick(findButton(dialog, "Warning"), Qt::LeftButton);

        REQUIRE(manager.posted().size() == 1);
        CHECK(manager.posted().back().type == NotificationType::Warning);
        CHECK(manager.posted().back().groupId == balloonGroupId);
    }

    SECTION("Error button posts an Error notification to the balloon group")
    {
        QTest::mouseClick(findButton(dialog, "Error"), Qt::LeftButton);

        REQUIRE(manager.posted().size() == 1);
        CHECK(manager.posted().back().type == NotificationType::Error);
        CHECK(manager.posted().back().groupId == balloonGroupId);
    }

    SECTION(
        "\"With 2 actions\" button posts a notification carrying two "
        "actions")
    {
        QTest::mouseClick(findButton(dialog, "With 2 actions"), Qt::LeftButton);

        REQUIRE(manager.posted().size() == 1);
        CHECK(manager.posted().back().groupId == balloonGroupId);
        REQUIRE(manager.posted().back().actions.size() == 2);
    }

    SECTION("Sticky button posts to the sticky balloon group")
    {
        QTest::mouseClick(findButton(dialog, "Sticky"), Qt::LeftButton);

        REQUIRE(manager.posted().size() == 1);
        CHECK(manager.posted().back().groupId == stickyGroupId);
    }
}
