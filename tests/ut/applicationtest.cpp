
#include <array>
#include <fstream>
#include <sstream>

#include <catch2/catch.hpp>

#include <QtCore/QStandardPaths>

#include <aide/application.hpp>

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

TEST_CASE("Application constructor throws an exception")
{
    // NOLINTNEXTLINE
    std::array<char*, 1> appName{{const_cast<char*>("aide_test")}};

    std::unique_ptr<aide::Application> app;

    SECTION(" if organization name is not set before calling")
    {
        aide::Application::setApplicationName("aide_test");
        aide::Application::setOrganizationName("");

        REQUIRE_THROWS_AS(
            std::make_unique<aide::Application>(1, appName.data()),
            std::runtime_error);
    }
}

TEST_CASE("Application constructor does not throw an exception")
{
    // NOLINTNEXTLINE
    std::array<char*, 1> appName{{const_cast<char*>("aide_test")}};

    std::unique_ptr<aide::Application> app;

    SECTION(" if organization name is set before calling")
    {
        aide::Application::setOrganizationName("aide_company");

        REQUIRE_NOTHROW(std::make_unique<aide::Application>(1, appName.data()));
    }
}

TEST_CASE("Logger logs into cache directory")
{
    aide::Application::setApplicationName("aide_test");
    aide::Application::setOrganizationName("aide_company");

    const std::string logFileLocation(
        QStandardPaths::writableLocation(QStandardPaths::CacheLocation)
            .append("/")
            .append(aide::Application::applicationName())
            .append(".log")
            .toStdString());

    std::remove(logFileLocation.c_str());

    // NOLINTNEXTLINE
    std::array<char*, 1> appName{{const_cast<char*>("aide_test")}};
    aide::Application app(1, appName.data());

    app.logger()->flush();
    REQUIRE(::lookForContentInFile(logFileLocation.c_str(), "Configured") !=
            std::string::npos);
}