
#include <array>
#include <fstream>
#include <sstream>

#include <catch2/catch_test_macros.hpp>

#include <QtCore/QStandardPaths>

#include <aide/application.hpp>
#include <aide/helptopicid.hpp>

#include "mockurllauncher.hpp"

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

    const auto launcher = std::make_shared<aide::tests::MockUrlLauncher>();

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

    const auto launcher = std::make_shared<aide::tests::MockUrlLauncher>();

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
