#include <catch2/catch_test_macros.hpp>

#include <QApplication>
#include <QCoreApplication>
#include <QPushButton>
#include <QSignalSpy>
#include <QToolButton>

#include <aide/hierarchicalid.hpp>

#include "aide/gui/widgets/gotittooltip.hpp"
#include "mocksettings.hpp"

using aide::test::MockSettings;
using aide::widgets::GotItPosition;
using aide::widgets::GotItTooltip;

namespace
{
    constexpr int PUMP_ITERATIONS{5};
    constexpr int TARGET_WIDTH{40};
    constexpr int TARGET_HEIGHT{20};
    constexpr int SIGNAL_WAIT_MS{1000};
    constexpr int SHORT_TIMEOUT_MS{1};
    constexpr int RAISED_MAX_COUNT{3};

    void pump()
    {
        for (int i = 0; i < PUMP_ITERATIONS; ++i) {
            QApplication::processEvents();
        }
    }

    void resizeToTargetSize(QWidget& widget)
    {
        widget.resize(TARGET_WIDTH, TARGET_HEIGHT);
    }
} // namespace

TEST_CASE("A GotItTooltip can be constructed for each position",
          "[GotItTooltip]")
{
    MockSettings settings;
    QWidget target;
    resizeToTargetSize(target);
    target.show();
    pump();

    for (auto position : {GotItPosition::Below, GotItPosition::Above,
                          GotItPosition::Left, GotItPosition::Right}) {
        auto* tooltip = new GotItTooltip(settings, "test.gotit.construct",
                                         "Something to learn");
        tooltip->showGotIt(&target, position);
        pump();
        REQUIRE(tooltip->isVisible());
        tooltip->close();
        delete tooltip;
    }
}

TEST_CASE("A GotItTooltip", "[GotItTooltip]")
{
    SECTION("can show once by default")
    {
        MockSettings settings;
        const GotItTooltip tooltip(settings, "test.gotit.default", "Body");
        REQUIRE(tooltip.canShow());
    }

    SECTION("cannot show once its show-count reaches the default max")
    {
        MockSettings settings;
        settings.setValue(
            aide::HierarchicalId("aide")("gotit")("test.gotit.seen"), 1);

        const GotItTooltip tooltip(settings, "test.gotit.seen", "Body");
        REQUIRE_FALSE(tooltip.canShow());
    }

    SECTION("withShowCount raises how many times it may show")
    {
        MockSettings settings;
        settings.setValue(
            aide::HierarchicalId("aide")("gotit")("test.gotit.raised"), 1);

        GotItTooltip tooltip(settings, "test.gotit.raised", "Body");
        tooltip.withShowCount(RAISED_MAX_COUNT);
        REQUIRE(tooltip.canShow());
    }

    SECTION("does not show and deletes itself when canShow() is false")
    {
        MockSettings settings;
        settings.setValue(
            aide::HierarchicalId("aide")("gotit")("test.gotit.blocked"), 1);

        QWidget target;
        resizeToTargetSize(target);
        target.show();
        pump();

        auto* tooltip =
            new GotItTooltip(settings, "test.gotit.blocked", "Body");
        tooltip->showGotIt(&target);
        pump();
        REQUIRE_FALSE(tooltip->isVisible());
    }
}

TEST_CASE("Dismissing a GotItTooltip via its \"Got it\" button",
          "[GotItTooltip]")
{
    MockSettings settings;
    QWidget target;
    resizeToTargetSize(target);
    target.show();
    pump();

    auto* tooltip = new GotItTooltip(settings, "test.gotit.dismiss", "Body");
    const QSignalSpy spy(tooltip, &GotItTooltip::closed);

    tooltip->showGotIt(&target);
    pump();
    REQUIRE(tooltip->isVisible());

    auto* gotItButton = tooltip->findChild<QPushButton*>();
    REQUIRE(gotItButton != nullptr);
    gotItButton->click();
    pump();

    REQUIRE(spy.count() == 1);
    REQUIRE(
        settings
            .value(aide::HierarchicalId("aide")("gotit")("test.gotit.dismiss"))
            .toInt() == 1);
}

TEST_CASE("A GotItTooltip defers showing until its target becomes visible",
          "[GotItTooltip]")
{
    MockSettings settings;
    QWidget target;
    resizeToTargetSize(target);

    auto* tooltip = new GotItTooltip(settings, "test.gotit.defer", "Body");
    tooltip->showGotIt(&target);
    pump();

    REQUIRE_FALSE(tooltip->isVisible());

    target.show();
    pump();

    REQUIRE(tooltip->isVisible());
    tooltip->close();
    delete tooltip;
}

TEST_CASE(
    "A GotItTooltip defers showing until its already-visible target "
    "has non-empty bounds",
    "[GotItTooltip]")
{
    MockSettings settings;
    QWidget target;
    target.resize(0, 0);
    target.show();
    pump();

    auto* tooltip =
        new GotItTooltip(settings, "test.gotit.emptybounds", "Body");
    tooltip->showGotIt(&target);
    pump();

    REQUIRE_FALSE(tooltip->isVisible());

    resizeToTargetSize(target);
    pump();

    REQUIRE(tooltip->isVisible());
    tooltip->close();
    delete tooltip;
}

TEST_CASE("withTimeout removes the \"Got it\" button and auto-dismisses",
          "[GotItTooltip]")
{
    MockSettings settings;
    QWidget target;
    resizeToTargetSize(target);
    target.show();
    pump();

    auto* tooltip = new GotItTooltip(settings, "test.gotit.timeout", "Body");
    tooltip->withTimeout(SHORT_TIMEOUT_MS);

    const auto* gotItButton = tooltip->findChild<QPushButton*>();
    REQUIRE(gotItButton != nullptr);
    REQUIRE_FALSE(gotItButton->isVisible());

    QSignalSpy spy(tooltip, &GotItTooltip::closed);
    tooltip->showGotIt(&target);

    REQUIRE(spy.wait(SIGNAL_WAIT_MS));
}

TEST_CASE("withLink invokes its handler and dismisses the tooltip",
          "[GotItTooltip]")
{
    MockSettings settings;
    QWidget target;
    resizeToTargetSize(target);
    target.show();
    pump();

    auto* tooltip = new GotItTooltip(settings, "test.gotit.link", "Body");

    bool handlerCalled = false;
    tooltip->withLink("Learn more",
                      [&handlerCalled]() { handlerCalled = true; });

    const QSignalSpy spy(tooltip, &GotItTooltip::closed);
    tooltip->showGotIt(&target);
    pump();

    auto* linkButton = tooltip->findChild<QToolButton*>();
    REQUIRE(linkButton != nullptr);
    linkButton->click();
    pump();

    REQUIRE(handlerCalled);
    REQUIRE(spy.count() == 1);
}

TEST_CASE(
    "Only one GotItTooltip shows at a time; closing one advances the "
    "queue",
    "[GotItTooltip]")
{
    MockSettings settings;
    QWidget target;
    resizeToTargetSize(target);
    target.show();
    pump();

    auto* first = new GotItTooltip(settings, "test.gotit.queue.first", "Body");
    auto* second =
        new GotItTooltip(settings, "test.gotit.queue.second", "Body");

    first->showGotIt(&target);
    second->showGotIt(&target);
    pump();

    REQUIRE(first->isVisible());
    REQUIRE_FALSE(second->isVisible());

    auto* firstButton = first->findChild<QPushButton*>();
    REQUIRE(firstButton != nullptr);
    firstButton->click();
    pump();

    REQUIRE(second->isVisible());
    second->close();
    delete second;
}

TEST_CASE("GotItTooltip::isFirstRunAfterUpgrade", "[GotItTooltip]")
{
    MockSettings settings;

    SECTION("is true the first time it is checked")
    {
        REQUIRE(GotItTooltip::isFirstRunAfterUpgrade(settings));
    }

    SECTION("is false on a repeat check with the same application version")
    {
        REQUIRE(GotItTooltip::isFirstRunAfterUpgrade(settings));
        REQUIRE_FALSE(GotItTooltip::isFirstRunAfterUpgrade(settings));
    }

    SECTION("is true again once the application version changes")
    {
        const auto originalVersion = QCoreApplication::applicationVersion();

        QCoreApplication::setApplicationVersion("1.0.0");
        REQUIRE(GotItTooltip::isFirstRunAfterUpgrade(settings));
        REQUIRE_FALSE(GotItTooltip::isFirstRunAfterUpgrade(settings));

        QCoreApplication::setApplicationVersion("2.0.0");
        REQUIRE(GotItTooltip::isFirstRunAfterUpgrade(settings));
        REQUIRE_FALSE(GotItTooltip::isFirstRunAfterUpgrade(settings));

        QCoreApplication::setApplicationVersion(originalVersion);
    }
}
