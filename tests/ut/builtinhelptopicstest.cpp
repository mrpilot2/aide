#include <memory>
#include <set>

#include <catch2/catch_test_macros.hpp>

#include <aide/appearancemanager.hpp>
#include <aide/helptopicid.hpp>
#include <aide/notificationmanager.hpp>

#include "actionregistry.hpp"
#include "mockkeymappagewidget.hpp"
#include "mocksettings.hpp"
#include "nulllogger.hpp"
#include "settings/appearancepage.hpp"
#include "settings/keymap/keymappage.hpp"
#include "settings/notifications/notificationssettingspage.hpp"

using aide::ActionRegistry;
using aide::AppearanceManager;
using aide::HelpTopicId;
using aide::NotificationManager;
using aide::core::KeymapPage;
using aide::gui::AppearancePage;
using aide::gui::NotificationsSettingsPage;
using aide::test::MockKeyMapPageWidget;
using aide::test::MockSettings;
using aide::test::NullLogger;

// Golden list: aIDE's built-in Help Topic IDs are stable public API. Renaming
// or removing one is a breaking change (feat! / BREAKING CHANGE), so update
// this list only together with such a release note.
TEST_CASE("Built-in settings pages declare exactly the stable help topics",
          "[HelpTopicId]")
{
    auto settings = std::make_shared<MockSettings>();

    AppearanceManager appearanceManager{settings};
    const AppearancePage appearancePage{appearanceManager};

    auto widget = std::make_shared<MockKeyMapPageWidget>();
    const KeymapPage keymapPage{std::make_shared<ActionRegistry>(
                                    *settings, std::make_shared<NullLogger>()),
                                widget.get()};

    NotificationManager notificationManager{*settings};
    const NotificationsSettingsPage notificationsPage{notificationManager,
                                                      *settings};

    const std::set<HelpTopicId> declared{appearancePage.helpTopicId(),
                                         keymapPage.helpTopicId(),
                                         notificationsPage.helpTopicId()};

    const std::set<HelpTopicId> expected{HelpTopicId{"appearance"},
                                         HelpTopicId{"keymap"},
                                         HelpTopicId{"notifications"}};

    REQUIRE(declared == expected);
}
