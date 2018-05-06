
#include "catch.hpp"

#include <aide/application.hpp>

TEST_CASE("Any application creates a main window")
{
    char* argv[] = {const_cast<char*>("a")};
    aide::Application app(1, argv);

    REQUIRE(app.mainWindow() != nullptr);
}
