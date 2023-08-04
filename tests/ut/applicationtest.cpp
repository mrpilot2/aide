
#include <array>
#include <fstream>
#include <sstream>

#include <catch2/catch.hpp>

#include <QtCore/QStandardPaths>

#include <aide/application.hpp>
#include <aide/settings/settingspageregistry.hpp>

using aide::core::SettingsPageRegistry;

namespace
{
    size_t lookForContentInFile(const char* const fileName,
                                const char* const searchString)
    {
        std::ifstream logFile(fileName, std::ios::in);
        std::stringstream fileContent;
        fileContent << logFile.rdbuf();
        return fileContent.str().find(searchString);
    }
} // namespace

TEST_CASE("Application constructor throws an exception", "[Application]")
{
    // NOLINTNEXTLINE
    std::array<char*, 1> appName{{const_cast<char*>("aide_test")}};

    std::unique_ptr<aide::Application> app;

    SECTION(" if organization name is not set before calling")
    {
        aide::Application::setApplicationName("aide_test");
        aide::Application::setOrganizationName("");

        int argc{1};
        REQUIRE_THROWS_AS(
            std::make_unique<aide::Application>(argc, appName.data()),
            std::runtime_error);
    }

    SettingsPageRegistry::deleteAllPages();
}

TEST_CASE("Application constructor does not throw an exception",
          "[Application]")
{
    // NOLINTNEXTLINE
    std::array<char*, 1> appName{{const_cast<char*>("aide_test")}};

    std::unique_ptr<aide::Application> app;

    SECTION(" if organization name is set before calling")
    {
        aide::Application::setOrganizationName("aide_company");

        int argc{1};
        REQUIRE_NOTHROW(
            std::make_unique<aide::Application>(argc, appName.data()));
    }

    SettingsPageRegistry::deleteAllPages();
}

TEST_CASE("Logger logs into cache directory", "[Application]")
{
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
    [[maybe_unused]] aide::Application app(numberOfArgs, appName.data());

    aide::Application::logger()->flush();
    REQUIRE(::lookForContentInFile(logFileLocation.c_str(), "Configured") !=
            std::string::npos);

    SettingsPageRegistry::deleteAllPages();
}

TEST_CASE("Application translator interface is never null", "[Application]")
{
    aide::Application::setApplicationName("aide_test");
    aide::Application::setOrganizationName("aide_company");

    int numberOfArgs{1};
    // NOLINTNEXTLINE
    std::array<char*, 1> appName{{const_cast<char*>("aide_test")}};
    aide::Application app(numberOfArgs, appName.data());

    REQUIRE(app.translator() != nullptr);

    SettingsPageRegistry::deleteAllPages();
}

TEST_CASE("Application main window is never null", "[Application]")
{
    aide::Application::setApplicationName("aide_test");
    aide::Application::setOrganizationName("aide_company");

    int numberOfArgs{1};
    // NOLINTNEXTLINE
    std::array<char*, 1> appName{{const_cast<char*>("aide_test")}};
    aide::Application app(numberOfArgs, appName.data());

    REQUIRE(app.mainWindow() != nullptr);

    SettingsPageRegistry::deleteAllPages();
}

TEST_CASE("Application settings provider is never null", "[Application]")
{
    aide::Application::setApplicationName("aide_test");
    aide::Application::setOrganizationName("aide_company");

    int numberOfArgs{1};
    // NOLINTNEXTLINE
    std::array<char*, 1> appName{{const_cast<char*>("aide_test")}};
    aide::Application app(numberOfArgs, appName.data());

    REQUIRE(app.settingsProvider() != nullptr);

    SettingsPageRegistry::deleteAllPages();
}
