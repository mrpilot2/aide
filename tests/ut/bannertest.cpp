#include <algorithm>

#include <catch2/catch_test_macros.hpp>

#include <QApplication>
#include <QSignalSpy>
#include <QToolButton>

#include <aide/notificationaction.hpp>
#include <aide/notificationtype.hpp>

#include "aide/gui/widgets/banner.hpp"

using aide::NotificationAction;
using aide::NotificationType;
using aide::widgets::Banner;

namespace
{
    constexpr int BANNER_WIDTH{400};
    constexpr int BANNER_HEIGHT{60};
} // namespace

TEST_CASE("A Banner can be constructed for each notification type", "[Banner]")
{
    for (auto type : {NotificationType::Information, NotificationType::Success,
                      NotificationType::Warning, NotificationType::Error}) {
        Banner banner(type, "Something happened");
        banner.resize(BANNER_WIDTH, BANNER_HEIGHT);
        banner.show();
        banner.repaint();
        QApplication::processEvents();
    }
}

TEST_CASE("A Banner", "[Banner]")
{
    SECTION("is closable by default")
    {
        const Banner banner(NotificationType::Information, "Message");

        const auto* closeButton = banner.findChild<QToolButton*>();
        REQUIRE(closeButton != nullptr);
    }

    SECTION("setClosable(false) hides the close button")
    {
        Banner banner(NotificationType::Information, "Message");
        banner.setClosable(false);

        const auto* closeButton = banner.findChild<QToolButton*>();
        REQUIRE(closeButton != nullptr);
        REQUIRE_FALSE(closeButton->isVisible());
    }
}

TEST_CASE("A Banner's close button", "[Banner]")
{
    const Banner banner(NotificationType::Information, "Message");

    const QSignalSpy spy(&banner, &Banner::closed);

    auto* closeButton = banner.findChild<QToolButton*>();
    REQUIRE(closeButton != nullptr);
    closeButton->click();

    REQUIRE(spy.count() == 1);
}

TEST_CASE("A Banner's action", "[Banner]")
{
    Banner banner(NotificationType::Information, "Message");

    bool handlerCalled = false;
    banner.addAction(NotificationAction{"Undo", [&handlerCalled]() {
                                            handlerCalled = true;
                                        }});

    const auto buttons = banner.findChildren<QToolButton*>();
    REQUIRE(buttons.size() == 2);

    const auto actionButtonIt = std::find_if(
        buttons.cbegin(), buttons.cend(),
        [](const QToolButton* button) { return button->text() == "Undo"; });
    REQUIRE(actionButtonIt != buttons.cend());

    (*actionButtonIt)->click();

    REQUIRE(handlerCalled);
}
