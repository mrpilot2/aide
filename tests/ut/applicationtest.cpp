
#include "catch.hpp"

#include <aide/application.hpp>

TEST_CASE("Any application creates a main window")
{
    int argc{1};
    char* argv[] = {const_cast<char*>("a")};
    aide::Application app(argc, argv);

    REQUIRE(app.mainWindow() != nullptr);
}
