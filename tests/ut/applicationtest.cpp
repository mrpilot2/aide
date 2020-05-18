
#include <array>

#include <QApplication>

#include <catch2/catch.hpp>

#include <aide/application.hpp>

TEST_CASE("Application constructor throws an exception")
{
    // NOLINTNEXTLINE
    std::array<char*, 1> appName{{const_cast<char*>("aide")}};

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
    std::array<char*, 1> appName{{const_cast<char*>("aide")}};

    std::unique_ptr<aide::Application> app;

    SECTION(" if organization name is set before calling")
    {
        aide::Application::setOrganizationName("MyCompany");

        REQUIRE_NOTHROW(
            std::make_unique<aide::Application>(1, appName.data()));
    }
}