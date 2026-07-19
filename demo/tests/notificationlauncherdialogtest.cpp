#include <optional>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include <QApplication>
#include <QComboBox>
#include <QGroupBox>
#include <QPushButton>
#include <QTest>

#include <aide/aidesettingsprovider.hpp>
#include <aide/gui/widgets/gotittooltip.hpp>
#include <aide/hierarchicalid.hpp>
#include <aide/notification.hpp>
#include <aide/notificationgroup.hpp>
#include <aide/notificationid.hpp>
#include <aide/notificationmanagerinterface.hpp>
#include <aide/settingsinterface.hpp>

#include "notificationlauncherdialog.hpp"

using aide::AideSettingsProvider;
using aide::HierarchicalId;
using aide::Notification;
using aide::NotificationDisplayType;
using aide::NotificationGroup;
using aide::NotificationId;
using aide::NotificationManagerInterface;
using aide::NotificationType;
using aide::widgets::GotItTooltip;
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

    constexpr int PUMP_ITERATIONS{5};

    void pump()
    {
        for (int i = 0; i < PUMP_ITERATIONS; ++i) {
            QApplication::processEvents();
        }
    }

    // Same key convention as gotittooltip.cpp's file-local gotItKey(): not
    // exported, so the demo section's fixed id has to be duplicated here to
    // reset the seen-count gate between SECTIONs sharing the process-wide
    // AideSettingsProvider (see catch_main.cpp).
    constexpr auto GOT_IT_DEMO_ID = "demo.launcher.gotit";

    void resetGotItSeenFlag()
    {
        if (const auto settings = AideSettingsProvider::unversionableSettings();
            settings != nullptr) {
            settings->removeKey(
                HierarchicalId("aide")("gotit")(GOT_IT_DEMO_ID));
        }
    }

    // The tooltip is a top-level QWidget with no parent (GotItTooltip's
    // default parent is nullptr), so it cannot be found via
    // dialog.findChild<>(). Dismissed/never-shown instances stay in
    // topLevelWidgets() until their deleteLater() is actually processed, so
    // only a visible one counts as "currently showing".
    GotItTooltip* findVisibleTooltip()
    {
        for (auto* widget : QApplication::topLevelWidgets()) {
            if (auto* tooltip = qobject_cast<GotItTooltip*>(widget);
                tooltip != nullptr && tooltip->isVisible()) {
                return tooltip;
            }
        }
        return nullptr;
    }

    // One position's worth of the "show every position" loop, pulled out of
    // its TEST_CASE so that function's cognitive complexity (clang-tidy caps
    // it around 25) stays under the for-loop-plus-several-REQUIRE budget.
    void showAndDismissGotIt(QComboBox& positionCombo, int index,
                             QPushButton& showButton, QPushButton& resetButton)
    {
        positionCombo.setCurrentIndex(index);
        QTest::mouseClick(&showButton, Qt::LeftButton);
        pump();

        auto* tooltip = findVisibleTooltip();
        REQUIRE(tooltip != nullptr);
        REQUIRE(tooltip->isVisible());

        // Dismiss via its own "Got it" button (rather than close()) so the
        // process-wide one-tooltip-at-a-time queue (see gotittooltip.cpp) is
        // left clean for the next iteration's Show click.
        auto* gotItButton = tooltip->findChild<QPushButton*>();
        REQUIRE(gotItButton != nullptr);
        QTest::mouseClick(gotItButton, Qt::LeftButton);
        pump();

        // The seen-count gate holds after one dismissal (verified by its own
        // TEST_CASE below); reset here so every position in this loop gets
        // its own Show.
        QTest::mouseClick(&resetButton, Qt::LeftButton);
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

TEST_CASE("A NotificationLauncherDialog has a Got it group box",
          "[NotificationLauncherDialog]")
{
    FakeNotificationManager manager;
    const HierarchicalId balloonGroupId{HierarchicalId("Demo")("Balloon")};
    const HierarchicalId stickyGroupId{
        HierarchicalId("Demo")("Sticky Balloon")};

    const NotificationLauncherDialog dialog(manager, balloonGroupId,
                                            stickyGroupId);

    bool found = false;
    for (const auto* group : dialog.findChildren<QGroupBox*>()) {
        if (group->title() == "Got it") { found = true; }
    }
    REQUIRE(found);
}

TEST_CASE("Show raises a GotItTooltip for every offered position",
          "[NotificationLauncherDialog]")
{
    resetGotItSeenFlag();

    FakeNotificationManager manager;
    const HierarchicalId balloonGroupId{HierarchicalId("Demo")("Balloon")};
    const HierarchicalId stickyGroupId{
        HierarchicalId("Demo")("Sticky Balloon")};

    NotificationLauncherDialog dialog(manager, balloonGroupId, stickyGroupId);
    dialog.show();
    [[maybe_unused]] const bool exposed = QTest::qWaitForWindowExposed(&dialog);
    pump();

    auto* showButton  = findButton(dialog, "Show");
    auto* resetButton = findButton(dialog, "Reset seen flag");
    REQUIRE(showButton != nullptr);
    REQUIRE(resetButton != nullptr);

    auto* positionCombo = dialog.findChild<QComboBox*>();
    REQUIRE(positionCombo != nullptr);
    REQUIRE(positionCombo->count() == 3);

    for (int index = 0; index < positionCombo->count(); ++index) {
        showAndDismissGotIt(*positionCombo, index, *showButton, *resetButton);
    }
}

TEST_CASE("The \"Got it\" section of a NotificationLauncherDialog",
          "[NotificationLauncherDialog]")
{
    resetGotItSeenFlag();

    FakeNotificationManager manager;
    const HierarchicalId balloonGroupId{HierarchicalId("Demo")("Balloon")};
    const HierarchicalId stickyGroupId{
        HierarchicalId("Demo")("Sticky Balloon")};

    NotificationLauncherDialog dialog(manager, balloonGroupId, stickyGroupId);
    dialog.show();
    [[maybe_unused]] const bool exposed = QTest::qWaitForWindowExposed(&dialog);
    pump();

    auto* showButton  = findButton(dialog, "Show");
    auto* resetButton = findButton(dialog, "Reset seen flag");
    REQUIRE(showButton != nullptr);
    REQUIRE(resetButton != nullptr);

    SECTION("Show does not reshow the tooltip once it has been dismissed")
    {
        QTest::mouseClick(showButton, Qt::LeftButton);
        pump();

        auto* tooltip = findVisibleTooltip();
        REQUIRE(tooltip != nullptr);
        auto* gotItButton = tooltip->findChild<QPushButton*>();
        REQUIRE(gotItButton != nullptr);
        QTest::mouseClick(gotItButton, Qt::LeftButton);
        pump();

        REQUIRE(findVisibleTooltip() == nullptr);

        QTest::mouseClick(showButton, Qt::LeftButton);
        pump();

        REQUIRE(findVisibleTooltip() == nullptr);
    }

    SECTION("Reset seen flag reshows the tooltip afterwards")
    {
        QTest::mouseClick(showButton, Qt::LeftButton);
        pump();

        auto* firstTooltip = findVisibleTooltip();
        REQUIRE(firstTooltip != nullptr);
        auto* gotItButton = firstTooltip->findChild<QPushButton*>();
        REQUIRE(gotItButton != nullptr);
        QTest::mouseClick(gotItButton, Qt::LeftButton);
        pump();

        REQUIRE(findVisibleTooltip() == nullptr);

        QTest::mouseClick(resetButton, Qt::LeftButton);
        QTest::mouseClick(showButton, Qt::LeftButton);
        pump();

        auto* secondTooltip = findVisibleTooltip();
        REQUIRE(secondTooltip != nullptr);
        REQUIRE(secondTooltip->isVisible());

        secondTooltip->close();
    }
}
