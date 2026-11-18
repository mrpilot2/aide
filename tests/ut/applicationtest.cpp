
#include <array>
#include <fstream>
#include <functional>
#include <memory>
#include <optional>
#include <sstream>
#include <utility>
#include <vector>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <QAction>
#include <QDialog>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QMenu>
#include <QPushButton>
#include <QTest>
#include <QTimer>
#include <QTreeView>
#include <QtCore/QStandardPaths>

#include <aide/aideconstants.hpp>
#include <aide/application.hpp>
#include <aide/helptopicid.hpp>
#include <aide/settings/settingspage.hpp>
#include <aide/settings/settingspageregistry.hpp>
#include <aide/widgethelptopic.hpp>

#include "mockurllauncher.hpp"

using aide::constants::CONSTANTS;

namespace
{
    size_t lookForContentInFile(const char* const fileName,
                                const char* const searchString)
    {
        const std::ifstream logFile(fileName, std::ios::in);
        std::stringstream fileContent;
        fileContent << logFile.rdbuf();
        return fileContent.str().find(searchString);
    }

    // Puts a focused line edit into the shown, active main window, so key
    // presses on it go through the main window's shortcuts.
    QLineEdit* focusLineEditInMainWindow(const aide::Application& app)
    {
        auto* mainWindow = app.mainWindow().get();
        auto* lineEdit   = new QLineEdit(mainWindow);
        mainWindow->setCentralWidget(lineEdit);
        mainWindow->show();
        mainWindow->activateWindow();
        REQUIRE(QTest::qWaitForWindowActive(mainWindow));
        lineEdit->setFocus();
        return lineEdit;
    }

    // A client settings page whose widget holds one line edit.
    class ClientPage : public aide::core::SettingsPage
    {
    public:
        ClientPage(aide::HierarchicalId group, aide::HelpTopicId topic)
            : SettingsPage(std::move(group))
            , m_topic(std::move(topic))
            , m_lineEdit(new QLineEdit(m_widget.get()))
        {}

        [[nodiscard]] aide::HelpTopicId helpTopicId() const override
        {
            return m_topic;
        }

        QWidget* widget() override { return m_widget.get(); }

        [[nodiscard]] QLineEdit* lineEdit() const { return m_lineEdit; }

        [[nodiscard]] bool isModified() const override { return false; }

        void reset() override {}

        void apply() override {}

    private:
        aide::HelpTopicId m_topic;
        std::unique_ptr<QWidget> m_widget{std::make_unique<QWidget>()};
        QLineEdit* m_lineEdit;
    };

    // Opens the settings dialog through File -> Settings, runs @p interact
    // once the modal dialog is shown and active, then cancels the dialog.
    // Runs inside the dialog's event loop, so @p interact records what it
    // observes instead of asserting.
    void interactWithSettingsDialog(
        const aide::Application& app,
        const std::function<void(QDialog&)>& interact)
    {
        const auto settings =
            app.actionRegistry()->action(CONSTANTS().FILE_SETTINGS);
        REQUIRE(settings.has_value());

        QTimer::singleShot(0, [&interact] {
            auto* dialog =
                qobject_cast<QDialog*>(QApplication::activeModalWidget());
            if (dialog == nullptr) { return; }
            if (QTest::qWaitForWindowActive(dialog)) { interact(*dialog); }
            dialog->reject();
        });
        // NOLINTNEXTLINE(bugprone-unchecked-optional-access)
        settings.value()->trigger();
    }

    // Selects the page titled @p title in the dialog's page tree and puts the
    // focus into the tree. Pages can hold tree views too (the Keymap does),
    // so the page tree is the one whose model has that title.
    void selectPageAndFocusTree(const QDialog& dialog, const QString& title)
    {
        for (auto* tree : dialog.findChildren<QTreeView*>()) {
            const auto* model = tree->model();
            const auto matches =
                model->match(model->index(0, 0), Qt::DisplayRole, title, 1,
                             Qt::MatchExactly | Qt::MatchRecursive);
            if (matches.isEmpty()) { continue; }

            tree->selectionModel()->setCurrentIndex(
                matches.first(), QItemSelectionModel::ClearAndSelect |
                                     QItemSelectionModel::Rows);
            tree->setFocus();
            return;
        }
    }

    bool isHelpButtonEnabled(const QDialog& dialog)
    {
        const auto* box = dialog.findChild<QDialogButtonBox*>("helpButtonBox");
        return box->button(QDialogButtonBox::Help)->isEnabled();
    }

    void pressInFocusWidget(const QKeySequence& keys)
    {
        QTest::keySequence(QApplication::focusWidget(), keys);
    }

    // An Application with help active, opening URLs through a mock launcher.
    struct HelpActiveApplication
    {
        static aide::ApplicationConfig prepare(
            const std::shared_ptr<aide::test::MockUrlLauncher>& launcher)
        {
            aide::Application::disableLoggingToConsole();
            aide::Application::setApplicationName("aide_test");
            aide::Application::setOrganizationName("aide_company");
            return aide::ApplicationConfig{}
                .setUrlLauncher(launcher)
                .setHelpBaseUrl(QUrl("https://x/help"));
        }

        std::shared_ptr<aide::test::MockUrlLauncher> launcher{
            std::make_shared<aide::test::MockUrlLauncher>()};
        int numberOfArgs{1};
        // NOLINTNEXTLINE
        std::array<char*, 1> appName{{const_cast<char*>("aide_test")}};
        aide::Application app{numberOfArgs, appName.data(), prepare(launcher)};
    };
} // namespace

TEST_CASE("Application constructor throws an exception", "[Application]")
{
    // NOLINTNEXTLINE
    std::array<char*, 1> appName{{const_cast<char*>("aide_test")}};

    aide::Application::disableLoggingToConsole();

    [[maybe_unused]] const std::unique_ptr<aide::Application> app;

    SECTION(" if organization name is not set before calling")
    {
        aide::Application::setApplicationName("aide_test");
        aide::Application::setOrganizationName("");

        int argc{1};
        REQUIRE_THROWS_AS(
            std::make_unique<aide::Application>(argc, appName.data()),
            std::runtime_error);
    }
}

TEST_CASE("Application constructor does not throw an exception",
          "[Application]")
{
    // NOLINTNEXTLINE
    std::array<char*, 1> appName{{const_cast<char*>("aide_test")}};

    aide::Application::disableLoggingToConsole();

    [[maybe_unused]] const std::unique_ptr<aide::Application> app;

    SECTION(" if organization name is set before calling")
    {
        aide::Application::setOrganizationName("aide_company");

        int argc{1};
        REQUIRE_NOTHROW(
            std::make_unique<aide::Application>(argc, appName.data()));
    }
}

TEST_CASE("Logger logs into cache directory", "[Application]")
{
    aide::Application::disableLoggingToConsole();

    aide::Application::setApplicationName("aide_test");
    aide::Application::setOrganizationName("aide_company");

    const std::string logFileLocation(
        QStandardPaths::writableLocation(QStandardPaths::CacheLocation)
            .append("/")
            .append(aide::Application::applicationName())
            .append(".log")
            .toStdString());

    [[maybe_unused]] auto res = std::remove(logFileLocation.c_str());

    int numberOfArgs{1};
    // NOLINTNEXTLINE
    std::array<char*, 1> appName{{const_cast<char*>("aide_test")}};
    [[maybe_unused]] const aide::Application app(numberOfArgs, appName.data());

    aide::Application::logger()->flush();
    REQUIRE(::lookForContentInFile(logFileLocation.c_str(), "Configured") !=
            std::string::npos);
}

TEST_CASE("Application translator interface is never null", "[Application]")
{
    aide::Application::disableLoggingToConsole();

    aide::Application::setApplicationName("aide_test");
    aide::Application::setOrganizationName("aide_company");

    int numberOfArgs{1};
    // NOLINTNEXTLINE
    std::array<char*, 1> appName{{const_cast<char*>("aide_test")}};
    const aide::Application app(numberOfArgs, appName.data());

    REQUIRE(app.translator() != nullptr);
}

TEST_CASE("Application main window is never null", "[Application]")
{
    aide::Application::disableLoggingToConsole();

    aide::Application::setApplicationName("aide_test");
    aide::Application::setOrganizationName("aide_company");

    int numberOfArgs{1};
    // NOLINTNEXTLINE
    std::array<char*, 1> appName{{const_cast<char*>("aide_test")}};
    const aide::Application app(numberOfArgs, appName.data());

    REQUIRE(app.mainWindow() != nullptr);

    auto* const banner = app.mainWindow()->addBanner(
        aide::NotificationType::Information, "banner message");
    REQUIRE(banner != nullptr);

    app.mainWindow()->removeBanner(banner);
}

TEST_CASE("Application settings provider is never null", "[Application]")
{
    aide::Application::disableLoggingToConsole();

    aide::Application::setApplicationName("aide_test");
    aide::Application::setOrganizationName("aide_company");

    int numberOfArgs{1};
    // NOLINTNEXTLINE
    std::array<char*, 1> appName{{const_cast<char*>("aide_test")}};
    const aide::Application app(numberOfArgs, appName.data());

    REQUIRE(app.settingsProvider() != nullptr);
}

TEST_CASE("Application enables every feature without a config", "[Application]")
{
    aide::Application::disableLoggingToConsole();

    aide::Application::setApplicationName("aide_test");
    aide::Application::setOrganizationName("aide_company");

    int numberOfArgs{1};
    // NOLINTNEXTLINE
    std::array<char*, 1> appName{{const_cast<char*>("aide_test")}};
    const aide::Application app(numberOfArgs, appName.data());

    REQUIRE(app.config().isEnabled(
        aide::ApplicationConfig::Feature::ViewFullscreenAction));
}

TEST_CASE("Application exposes the config it was constructed with",
          "[Application]")
{
    aide::Application::disableLoggingToConsole();

    aide::Application::setApplicationName("aide_test");
    aide::Application::setOrganizationName("aide_company");

    int numberOfArgs{1};
    // NOLINTNEXTLINE
    std::array<char*, 1> appName{{const_cast<char*>("aide_test")}};
    const aide::Application app(
        numberOfArgs, appName.data(),
        aide::ApplicationConfig{}.setEnabled(
            aide::ApplicationConfig::Feature::ViewFullscreenAction, false));

    REQUIRE_FALSE(app.config().isEnabled(
        aide::ApplicationConfig::Feature::ViewFullscreenAction));
}

TEST_CASE("Application shows help through the configured URL launcher",
          "[Application]")
{
    aide::Application::disableLoggingToConsole();

    aide::Application::setApplicationName("aide_test");
    aide::Application::setOrganizationName("aide_company");

    const auto launcher = std::make_shared<aide::test::MockUrlLauncher>();

    int numberOfArgs{1};
    // NOLINTNEXTLINE
    std::array<char*, 1> appName{{const_cast<char*>("aide_test")}};
    aide::Application app(numberOfArgs, appName.data(),
                          aide::ApplicationConfig{}
                              .setUrlLauncher(launcher)
                              .setHelpBaseUrl(QUrl("https://x/help"))
                              .disableHelpTopic(aide::HelpTopicId{"off"}));

    SECTION("for an available topic")
    {
        REQUIRE(app.isHelpAvailable(aide::HelpTopicId{"keymap"}));
        REQUIRE(app.showHelp(aide::HelpTopicId{"keymap"}));
        REQUIRE(launcher->lastRequestedUrl ==
                std::string("https://x/help/keymap/"));
    }

    SECTION("for Help Contents")
    {
        REQUIRE(app.showHelpContents());
        REQUIRE(launcher->lastRequestedUrl == std::string("https://x/help/"));
    }

    SECTION("but not for a disabled topic")
    {
        REQUIRE_FALSE(app.isHelpAvailable(aide::HelpTopicId{"off"}));
        REQUIRE_FALSE(app.showHelp(aide::HelpTopicId{"off"}));
        REQUIRE_FALSE(launcher->lastRequestedUrl.has_value());
    }
}

TEST_CASE("Application help is inactive without a Help Base URL",
          "[Application]")
{
    aide::Application::disableLoggingToConsole();

    aide::Application::setApplicationName("aide_test");
    aide::Application::setOrganizationName("aide_company");

    const auto launcher = std::make_shared<aide::test::MockUrlLauncher>();

    int numberOfArgs{1};
    // NOLINTNEXTLINE
    std::array<char*, 1> appName{{const_cast<char*>("aide_test")}};
    aide::Application app(numberOfArgs, appName.data(),
                          aide::ApplicationConfig{}.setUrlLauncher(launcher));

    REQUIRE_FALSE(app.isHelpAvailable(aide::HelpTopicId{"keymap"}));
    REQUIRE_FALSE(app.showHelp(aide::HelpTopicId{"keymap"}));
    REQUIRE_FALSE(app.showHelpContents());
    REQUIRE_FALSE(launcher->lastRequestedUrl.has_value());
}

TEST_CASE("Application with help active offers Help Contents", "[Application]")
{
    aide::Application::disableLoggingToConsole();

    aide::Application::setApplicationName("aide_test");
    aide::Application::setOrganizationName("aide_company");

    const auto launcher = std::make_shared<aide::test::MockUrlLauncher>();

    int numberOfArgs{1};
    // NOLINTNEXTLINE
    std::array<char*, 1> appName{{const_cast<char*>("aide_test")}};
    const aide::Application app(
        numberOfArgs, appName.data(),
        aide::ApplicationConfig{}.setUrlLauncher(launcher).setHelpBaseUrl(
            QUrl("https://x/help")));

    const auto registry = app.actionRegistry();
    const auto action   = registry->action(CONSTANTS().HELP_CONTENTS);
    REQUIRE(action.has_value());

    SECTION("as the first Help menu entry")
    {
        const auto menuHelpContainer =
            registry->getMenuContainer(CONSTANTS().MENU_HELP);
        REQUIRE(menuHelpContainer.has_value());
        // NOLINTNEXTLINE(bugprone-unchecked-optional-access)
        const auto* menuHelp = menuHelpContainer.value()->menu();

        // NOLINTNEXTLINE(bugprone-unchecked-optional-access)
        REQUIRE(menuHelp->actions().first() == action.value());
    }

    SECTION("that opens the Help Base URL root")
    {
        // NOLINTNEXTLINE(bugprone-unchecked-optional-access)
        action.value()->trigger();

        REQUIRE(launcher->lastRequestedUrl == std::string("https://x/help/"));
    }

    SECTION("without a default shortcut")
    {
        REQUIRE(registry->actions()
                    .at(CONSTANTS().HELP_CONTENTS)
                    .defaultKeySequences.empty());
    }

    SECTION("that a user-assigned shortcut triggers")
    {
        const QKeySequence userShortcut(Qt::CTRL | Qt::SHIFT | Qt::Key_F1);
        registry->modifyShortcutsForAction(CONSTANTS().HELP_CONTENTS,
                                           {userShortcut});

        auto* mainWindow = app.mainWindow().get();
        mainWindow->show();
        mainWindow->activateWindow();
        const bool isActive = QTest::qWaitForWindowActive(mainWindow);

        QTest::keyClick(mainWindow, Qt::Key_F1,
                        Qt::ControlModifier | Qt::ShiftModifier);
        const auto requestedUrl = launcher->lastRequestedUrl;

        // Back to the (empty) default, so the persisted Keymap entry is
        // removed again and does not leak into later runs.
        registry->modifyShortcutsForAction(CONSTANTS().HELP_CONTENTS, {});

        REQUIRE(isActive);
        REQUIRE(requestedUrl == std::string("https://x/help/"));
    }
}

TEST_CASE("Application with help inactive offers no Help Contents",
          "[Application]")
{
    aide::Application::disableLoggingToConsole();

    aide::Application::setApplicationName("aide_test");
    aide::Application::setOrganizationName("aide_company");

    const auto config = GENERATE(
        aide::ApplicationConfig{},
        aide::ApplicationConfig{}
            .setHelpBaseUrl(QUrl("https://x/help"))
            .setEnabled(aide::ApplicationConfig::Feature::Help, false));

    int numberOfArgs{1};
    // NOLINTNEXTLINE
    std::array<char*, 1> appName{{const_cast<char*>("aide_test")}};
    const aide::Application app(numberOfArgs, appName.data(), config);

    const auto registry = app.actionRegistry();

    SECTION("in the Keymap")
    {
        REQUIRE_FALSE(registry->actions().contains(CONSTANTS().HELP_CONTENTS));
    }

    SECTION("in the Help menu, which starts with About aIDE instead")
    {
        const auto menuHelpContainer =
            registry->getMenuContainer(CONSTANTS().MENU_HELP);
        REQUIRE(menuHelpContainer.has_value());
        // NOLINTNEXTLINE(bugprone-unchecked-optional-access)
        const auto* menuHelp = menuHelpContainer.value()->menu();
        const auto aboutAide = registry->action(CONSTANTS().HELP_ABOUT_AIDE);
        REQUIRE(aboutAide.has_value());

        // NOLINTNEXTLINE(bugprone-unchecked-optional-access)
        REQUIRE(menuHelp->actions().first() == aboutAide.value());
    }
}

TEST_CASE("Application with help active offers Context Help", "[Application]")
{
    aide::Application::disableLoggingToConsole();

    aide::Application::setApplicationName("aide_test");
    aide::Application::setOrganizationName("aide_company");

    const auto launcher = std::make_shared<aide::test::MockUrlLauncher>();

    int numberOfArgs{1};
    // NOLINTNEXTLINE
    std::array<char*, 1> appName{{const_cast<char*>("aide_test")}};
    const aide::Application app(
        numberOfArgs, appName.data(),
        aide::ApplicationConfig{}.setUrlLauncher(launcher).setHelpBaseUrl(
            QUrl("https://x/help")));

    const auto registry = app.actionRegistry();
    const auto action   = registry->action(CONSTANTS().HELP_CONTEXT_HELP);
    REQUIRE(action.has_value());

    SECTION("with the platform's Help Contents key as default shortcut")
    {
        REQUIRE(registry->actions()
                    .at(CONSTANTS().HELP_CONTEXT_HELP)
                    .defaultKeySequences ==
                QList<QKeySequence>{QKeySequence(QKeySequence::HelpContents)});
    }

    SECTION("on the main window but not in the Help menu")
    {
        const auto menuHelpContainer =
            registry->getMenuContainer(CONSTANTS().MENU_HELP);
        REQUIRE(menuHelpContainer.has_value());
        // NOLINTNEXTLINE(bugprone-unchecked-optional-access)
        const auto* menuHelp = menuHelpContainer.value()->menu();

        // NOLINTBEGIN(bugprone-unchecked-optional-access)
        REQUIRE(app.mainWindow()->actions().contains(action.value()));
        REQUIRE_FALSE(menuHelp->actions().contains(action.value()));
        // NOLINTEND(bugprone-unchecked-optional-access)
    }

    SECTION("whose shortcut opens the focused widget's topic")
    {
        auto* lineEdit = focusLineEditInMainWindow(app);
        aide::setHelpTopic(lineEdit, aide::HelpTopicId{"workspace"});

        QTest::keySequence(lineEdit, QKeySequence(QKeySequence::HelpContents));

        REQUIRE(launcher->lastRequestedUrl ==
                std::string("https://x/help/workspace/"));
    }
}

TEST_CASE("Remapping Context Help changes the key that triggers it",
          "[Application]")
{
    aide::Application::disableLoggingToConsole();

    aide::Application::setApplicationName("aide_test");
    aide::Application::setOrganizationName("aide_company");

    const auto launcher = std::make_shared<aide::test::MockUrlLauncher>();

    int numberOfArgs{1};
    // NOLINTNEXTLINE
    std::array<char*, 1> appName{{const_cast<char*>("aide_test")}};
    const aide::Application app(
        numberOfArgs, appName.data(),
        aide::ApplicationConfig{}.setUrlLauncher(launcher).setHelpBaseUrl(
            QUrl("https://x/help")));

    const auto registry = app.actionRegistry();
    const QKeySequence userShortcut(Qt::CTRL | Qt::SHIFT | Qt::Key_H);
    registry->modifyShortcutsForAction(CONSTANTS().HELP_CONTEXT_HELP,
                                       {userShortcut});

    auto* lineEdit = focusLineEditInMainWindow(app);

    QTest::keySequence(lineEdit, QKeySequence(QKeySequence::HelpContents));
    const auto urlAfterDefaultKey = launcher->lastRequestedUrl;

    QTest::keySequence(lineEdit, userShortcut);
    const auto urlAfterUserKey = launcher->lastRequestedUrl;

    // Back to the default, so the persisted Keymap entry is removed again
    // and does not leak into later runs.
    registry->modifyShortcutsForAction(
        CONSTANTS().HELP_CONTEXT_HELP,
        {QKeySequence(QKeySequence::HelpContents)});

    REQUIRE_FALSE(urlAfterDefaultKey.has_value());
    REQUIRE(urlAfterUserKey == std::string("https://x/help/"));
}

TEST_CASE("Application with help inactive offers no Context Help",
          "[Application]")
{
    aide::Application::disableLoggingToConsole();

    aide::Application::setApplicationName("aide_test");
    aide::Application::setOrganizationName("aide_company");

    const auto launcher = std::make_shared<aide::test::MockUrlLauncher>();

    const auto config = GENERATE_COPY(
        aide::ApplicationConfig{}.setUrlLauncher(launcher),
        aide::ApplicationConfig{}
            .setUrlLauncher(launcher)
            .setHelpBaseUrl(QUrl("https://x/help"))
            .setEnabled(aide::ApplicationConfig::Feature::Help, false));

    int numberOfArgs{1};
    // NOLINTNEXTLINE
    std::array<char*, 1> appName{{const_cast<char*>("aide_test")}};
    const aide::Application app(numberOfArgs, appName.data(), config);

    SECTION("in the Keymap")
    {
        REQUIRE_FALSE(app.actionRegistry()->actions().contains(
            CONSTANTS().HELP_CONTEXT_HELP));
    }

    SECTION("so the default Context Help key does nothing")
    {
        auto* lineEdit = focusLineEditInMainWindow(app);
        aide::setHelpTopic(lineEdit, aide::HelpTopicId{"workspace"});

        QTest::keySequence(lineEdit, QKeySequence(QKeySequence::HelpContents));

        REQUIRE_FALSE(launcher->lastRequestedUrl.has_value());
    }
}

TEST_CASE("Application declares the built-in settings pages' Help Topics",
          "[Application]")
{
    aide::Application::disableLoggingToConsole();

    aide::Application::setApplicationName("aide_test");
    aide::Application::setOrganizationName("aide_company");

    using aide::HelpTopicId;

    // Help needs neither a base URL nor Feature::Help to list its topics.
    auto config = GENERATE(aide::ApplicationConfig{},
                           aide::ApplicationConfig{}.setEnabled(
                               aide::ApplicationConfig::Feature::Help, false));

    int numberOfArgs{1};
    // NOLINTNEXTLINE
    std::array<char*, 1> appName{{const_cast<char*>("aide_test")}};
    const aide::Application app(numberOfArgs, appName.data(), config);

    REQUIRE(app.declaredHelpTopics() ==
            std::vector{HelpTopicId{"appearance"}, HelpTopicId{"keymap"},
                        HelpTopicId{"notifications"}});
}

TEST_CASE("Application's declared Help Topics follow the config",
          "[Application]")
{
    aide::Application::disableLoggingToConsole();

    aide::Application::setApplicationName("aide_test");
    aide::Application::setOrganizationName("aide_company");

    using aide::HelpTopicId;

    struct Case
    {
        aide::ApplicationConfig config;
        std::vector<HelpTopicId> expected;
    };

    const auto [config, expected] = GENERATE(
        Case{aide::ApplicationConfig{}.disableHelpTopic(HelpTopicId{"keymap"}),
             {HelpTopicId{"appearance"}, HelpTopicId{"notifications"}}},
        Case{aide::ApplicationConfig{}.remapHelpTopic(
                 HelpTopicId{"keymap"}, HelpTopicId{"settings/keys"}),
             {HelpTopicId{"appearance"}, HelpTopicId{"notifications"},
              HelpTopicId{"settings/keys"}}},
        Case{aide::ApplicationConfig{}
                 .remapHelpTopic(HelpTopicId{"keymap"}, HelpTopicId{"settings"})
                 .remapHelpTopic(HelpTopicId{"appearance"},
                                 HelpTopicId{"settings"}),
             {HelpTopicId{"notifications"}, HelpTopicId{"settings"}}},
        Case{aide::ApplicationConfig{}.declareHelpTopic(
                 HelpTopicId{"editor/tabs"}),
             {HelpTopicId{"appearance"}, HelpTopicId{"editor/tabs"},
              HelpTopicId{"keymap"}, HelpTopicId{"notifications"}}});

    int numberOfArgs{1};
    // NOLINTNEXTLINE
    std::array<char*, 1> appName{{const_cast<char*>("aide_test")}};
    const aide::Application app(numberOfArgs, appName.data(), config);

    REQUIRE(app.declaredHelpTopics() == expected);
}

TEST_CASE_METHOD(HelpActiveApplication,
                 "Context Help in the settings dialog resolves focus in the "
                 "page tree to the selected page",
                 "[Application]")
{
    interactWithSettingsDialog(app, [](const QDialog& dialog) {
        selectPageAndFocusTree(dialog, "Keymap");
        pressInFocusWidget(QKeySequence(QKeySequence::HelpContents));
    });

    REQUIRE(launcher->lastRequestedUrl ==
            std::string("https://x/help/keymap/"));
}

TEST_CASE_METHOD(HelpActiveApplication,
                 "Context Help in the settings dialog prefers the topic of a "
                 "focused widget inside the page",
                 "[Application]")
{
    const auto page = std::make_shared<ClientPage>(
        aide::HierarchicalId("Client Page"), aide::HelpTopicId{"client-page"});
    aide::setHelpTopic(page->lineEdit(),
                       aide::HelpTopicId{"client-page/field"});
    app.settingsPageRegistry().addPage(page);

    interactWithSettingsDialog(app, [&page](const QDialog& dialog) {
        selectPageAndFocusTree(dialog, "Client Page");
        page->lineEdit()->setFocus();
        pressInFocusWidget(QKeySequence(QKeySequence::HelpContents));
    });

    REQUIRE(launcher->lastRequestedUrl ==
            std::string("https://x/help/client-page/field/"));
}

TEST_CASE_METHOD(HelpActiveApplication,
                 "Context Help in the settings dialog falls back to Help "
                 "Contents on a page without a topic",
                 "[Application]")
{
    app.settingsPageRegistry().addPage(std::make_shared<ClientPage>(
        aide::HierarchicalId("Client Page"), aide::HelpTopicId{}));
    bool helpButtonEnabled{true};

    interactWithSettingsDialog(
        app, [&helpButtonEnabled](const QDialog& dialog) {
            selectPageAndFocusTree(dialog, "Client Page");
            helpButtonEnabled = isHelpButtonEnabled(dialog);
            pressInFocusWidget(QKeySequence(QKeySequence::HelpContents));
        });

    REQUIRE_FALSE(helpButtonEnabled);
    REQUIRE(launcher->lastRequestedUrl == std::string("https://x/help/"));
}

TEST_CASE_METHOD(HelpActiveApplication,
                 "Remapped Context Help works in a newly opened settings "
                 "dialog",
                 "[Application]")
{
    const QKeySequence defaultKey(QKeySequence::HelpContents);
    const QKeySequence userKey(Qt::CTRL | Qt::SHIFT | Qt::Key_H);

    // Opened once before remapping, so the remap must reach a dialog that
    // already exists.
    interactWithSettingsDialog(app, [](const QDialog& /*dialog*/) {});

    const auto registry = app.actionRegistry();
    registry->modifyShortcutsForAction(CONSTANTS().HELP_CONTEXT_HELP,
                                       {userKey});

    std::optional<std::string> urlAfterDefaultKey;
    interactWithSettingsDialog(app, [&](const QDialog& dialog) {
        selectPageAndFocusTree(dialog, "Keymap");
        pressInFocusWidget(defaultKey);
        urlAfterDefaultKey = launcher->lastRequestedUrl;
        pressInFocusWidget(userKey);
    });

    // Back to the default, so the persisted Keymap entry is removed again
    // and does not leak into later runs.
    registry->modifyShortcutsForAction(CONSTANTS().HELP_CONTEXT_HELP,
                                       {defaultKey});

    REQUIRE_FALSE(urlAfterDefaultKey.has_value());
    REQUIRE(launcher->lastRequestedUrl ==
            std::string("https://x/help/keymap/"));
}

TEST_CASE("Settings dialog has no Context Help while help is inactive",
          "[Application]")
{
    aide::Application::disableLoggingToConsole();

    aide::Application::setApplicationName("aide_test");
    aide::Application::setOrganizationName("aide_company");

    const auto launcher = std::make_shared<aide::test::MockUrlLauncher>();

    int numberOfArgs{1};
    // NOLINTNEXTLINE
    std::array<char*, 1> appName{{const_cast<char*>("aide_test")}};
    const aide::Application app(
        numberOfArgs, appName.data(),
        aide::ApplicationConfig{}.setUrlLauncher(launcher));

    interactWithSettingsDialog(app, [](const QDialog& dialog) {
        selectPageAndFocusTree(dialog, "Keymap");
        pressInFocusWidget(QKeySequence(QKeySequence::HelpContents));
    });

    REQUIRE_FALSE(launcher->lastRequestedUrl.has_value());
}
