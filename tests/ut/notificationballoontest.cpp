#include <algorithm>

#include <catch2/catch_test_macros.hpp>

#include <QApplication>
#include <QMenu>
#include <QProgressBar>
#include <QSignalSpy>
#include <QToolButton>

#include <aide/notificationaction.hpp>
#include <aide/notificationtype.hpp>

#include "aide/gui/widgets/notificationballoon.hpp"

using aide::NotificationAction;
using aide::NotificationType;
using aide::widgets::NotificationBalloon;

namespace
{
    constexpr int BALLOON_HEIGHT{120};
} // namespace

TEST_CASE("A NotificationBalloon can be constructed for each notification type",
          "[NotificationBalloon]")
{
    for (auto type : {NotificationType::Information, NotificationType::Success,
                      NotificationType::Warning, NotificationType::Error}) {
        NotificationBalloon balloon(type, "Title", "Something happened", false);
        balloon.resize(balloon.width(), BALLOON_HEIGHT);
        balloon.show();
        balloon.repaint();
        QApplication::processEvents();
    }
}

TEST_CASE("A transient NotificationBalloon", "[NotificationBalloon]")
{
    SECTION("shows a countdown progress bar")
    {
        const NotificationBalloon balloon(NotificationType::Information,
                                          "Title", "Message", false);

        REQUIRE(balloon.findChild<QProgressBar*>() != nullptr);
    }
}

TEST_CASE("A sticky NotificationBalloon", "[NotificationBalloon]")
{
    SECTION("shows no countdown progress bar")
    {
        const NotificationBalloon balloon(NotificationType::Information,
                                          "Title", "Message", true);

        REQUIRE(balloon.findChild<QProgressBar*>() == nullptr);
    }
}

TEST_CASE("A NotificationBalloon's close button", "[NotificationBalloon]")
{
    const NotificationBalloon balloon(NotificationType::Information, "Title",
                                      "Message", true);

    const QSignalSpy spy(&balloon, &NotificationBalloon::closed);

    auto* closeButton = balloon.findChild<QToolButton*>();
    REQUIRE(closeButton != nullptr);
    closeButton->click();

    REQUIRE(spy.count() == 1);
}

TEST_CASE("Up to two NotificationBalloon actions show as inline buttons",
          "[NotificationBalloon]")
{
    NotificationBalloon balloon(NotificationType::Information, "Title",
                                "Message", true);

    bool firstCalled  = false;
    bool secondCalled = false;
    balloon.addAction(
        NotificationAction{.title = "First", .handler = [&firstCalled]() {
                               firstCalled = true;
                           }});
    balloon.addAction(
        NotificationAction{.title = "Second", .handler = [&secondCalled]() {
                               secondCalled = true;
                           }});

    const auto buttons    = balloon.findChildren<QToolButton*>();
    const auto findByText = [&buttons](const QString& text) {
        return std::ranges::find_if(buttons,
                                    [&text](const QToolButton* button) {
                                        return button->text() == text;
                                    });
    };

    const auto firstIt = findByText("First");
    REQUIRE(firstIt != buttons.cend());
    (*firstIt)->click();
    REQUIRE(firstCalled);

    const auto secondIt = findByText("Second");
    REQUIRE(secondIt != buttons.cend());
    (*secondIt)->click();
    REQUIRE(secondCalled);

    REQUIRE(findByText("More ▾") == buttons.cend());
}

TEST_CASE(
    "A third NotificationBalloon action moves under a More menu instead of "
    "inline",
    "[NotificationBalloon]")
{
    NotificationBalloon balloon(NotificationType::Information, "Title",
                                "Message", true);

    balloon.addAction(NotificationAction{.title = "First", .handler = []() {
                                         }});
    balloon.addAction(NotificationAction{.title = "Second", .handler = []() {
                                         }});

    bool thirdCalled = false;
    balloon.addAction(
        NotificationAction{.title = "Third", .handler = [&thirdCalled]() {
                               thirdCalled = true;
                           }});

    const auto buttons = balloon.findChildren<QToolButton*>();
    const auto moreIt  = std::ranges::find_if(
        buttons,
        [](const QToolButton* button) { return button->text() == "More ▾"; });
    REQUIRE(moreIt != buttons.cend());

    const auto thirdIt = std::ranges::find_if(
        buttons,
        [](const QToolButton* button) { return button->text() == "Third"; });
    REQUIRE(thirdIt == buttons.cend());

    (*moreIt)->menu()->actions().first()->trigger();
    REQUIRE(thirdCalled);
}
