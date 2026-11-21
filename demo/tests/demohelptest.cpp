#include <algorithm>
#include <array>
#include <memory>
#include <string>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include <QAction>
#include <QDir>
#include <QFileInfo>
#include <QStringList>
#include <QTest>
#include <QWidget>

#include <aide/aideconstants.hpp>
#include <aide/application.hpp>
#include <aide/gui/widgets/notificationview.hpp>
#include <aide/helptopicid.hpp>
#include <aide/settings/settingspage.hpp>

#include "colorschemereactor.hpp"
#include "demosetup.hpp"
#include "mockurllauncher.hpp"

using aide::HelpTopicId;
using aide::constants::CONSTANTS;

namespace
{
    // Directory of the built demo End-User Help site (mkdocs `site_dir`).
    // The coverage test is skipped when it is unset, so plain ctest runs
    // do not need mkdocs.
    constexpr const char* SITE_DIR_VARIABLE{"AIDE_DEMO_HELP_SITE_DIR"};

    // The demo application exactly as main() builds it, except that URLs
    // go to a mock launcher instead of the system browser.
    struct ApplicationNames
    {
        ApplicationNames()
        {
            aide::Application::disableLoggingToConsole();
            aide::Application::setOrganizationName("aide_company");
            aide::Application::setApplicationName("aide_test");
        }
    };

    struct DemoApplication : ApplicationNames
    {
        std::shared_ptr<aide::test::MockUrlLauncher> browser{
            std::make_shared<aide::test::MockUrlLauncher>()};
        int numberOfArgs{1};
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
        std::array<char*, 1> appName{{const_cast<char*>("aide_demo")}};
        aide::Application app{numberOfArgs, appName.data(),
                              demo::demoConfig(browser)};
        std::unique_ptr<demo::ColorSchemeReactor> reactor{demo::setUpDemo(app)};
    };

    bool contains(const std::vector<HelpTopicId>& topics, const char* id)
    {
        return std::ranges::find(topics, HelpTopicId{id}) != topics.end();
    }
} // namespace

TEST_CASE("Demo declares its own and the built-in Help Topics",
          "[Application][Help]")
{
    const DemoApplication demo;

    const auto topics = demo.app.declaredHelpTopics();

    REQUIRE_FALSE(topics.empty());
    REQUIRE(contains(topics, "demo-settings"));
    REQUIRE(contains(topics, "demo-workspace"));
    REQUIRE(contains(topics, "appearance"));
    REQUIRE(contains(topics, "keymap"));
    REQUIRE(contains(topics, "notifications"));
}

TEST_CASE("Every demo Help Topic has a page in the demo help site",
          "[Application][Help]")
{
    const auto siteDir = qEnvironmentVariable(SITE_DIR_VARIABLE);
    if (siteDir.isEmpty()) {
        SKIP(SITE_DIR_VARIABLE
             << " is not set; build the demo help site and point it at the "
                "site directory to run this check");
    }

    const DemoApplication demo;
    const auto topics = demo.app.declaredHelpTopics();
    REQUIRE_FALSE(topics.empty());

    QStringList missing;
    for (const auto& topic : topics) {
        const QFileInfo page(
            QDir(siteDir).filePath(topic.value() + "/index.html"));
        if (!page.isFile()) { missing << topic.value(); }
    }

    INFO("site directory: " << siteDir.toStdString());
    INFO("Help Topics without a page: " << missing.join(", ").toStdString());
    REQUIRE(missing.isEmpty());
}

TEST_CASE("Demo Help Contents opens the root of the demo-help site",
          "[Application][Help]")
{
    const DemoApplication demo;

    const auto action =
        demo.app.actionRegistry()->action(CONSTANTS().HELP_CONTENTS);
    REQUIRE(action.has_value());

    // NOLINTNEXTLINE(bugprone-unchecked-optional-access)
    action.value()->trigger();

    REQUIRE(demo.browser->lastRequestedUrl ==
            std::string("https://mrpilot2.github.io/aide/demo-help/"));
}

// The settings dialog's Help button opens the selected page's helpTopicId()
// (covered by aIDE's own tests), so the demo only has to give its pages a
// topic that help can open.
TEST_CASE("Demo settings pages open the demo-settings Help Page",
          "[Application][Help]")
{
    DemoApplication demo;

    // Both "Demo Subpage 1.x" pages; the other pages are aIDE's own.
    constexpr auto demoPageCount{2};
    const auto& pages = demo.app.settingsPageRegistry().settingsPages();
    REQUIRE(std::ranges::count_if(pages, [](const auto& page) {
                return page->helpTopicId() == HelpTopicId{"demo-settings"};
            }) == demoPageCount);

    REQUIRE(demo.app.showHelp(HelpTopicId{"demo-settings"}));
    REQUIRE(demo.browser->lastRequestedUrl ==
            std::string(
                "https://mrpilot2.github.io/aide/demo-help/demo-settings/"));
}

TEST_CASE("Demo Context Help in the workspace opens the demo-workspace page",
          "[Application][Help]")
{
    const DemoApplication demo;

    auto* mainWindow = demo.app.mainWindow().get();
    mainWindow->show();
    mainWindow->activateWindow();
    REQUIRE(QTest::qWaitForWindowActive(mainWindow));

    // Any widget inside the central splitter inherits its topic.
    mainWindow->notificationLogView()->setFocus();

    QTest::keySequence(mainWindow, QKeySequence(QKeySequence::HelpContents));

    REQUIRE(demo.browser->lastRequestedUrl ==
            std::string(
                "https://mrpilot2.github.io/aide/demo-help/demo-workspace/"));
}
