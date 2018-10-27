
#include "catch.hpp"

#include <aide/commandmanager/commandmanager.hpp>

TEST_CASE("A new command manager")
{
    SECTION(" contains no commands")
    {
        aide::CommandManager commandManager;

        REQUIRE(commandManager.commands().isEmpty());
    }
}

TEST_CASE("Any command manager")
{
    aide::CommandManager commandManager;

    SECTION(" throws an exception if a null action is added")
    {
        REQUIRE_THROWS_AS(commandManager.registerAction(nullptr), std::invalid_argument);
    }
}
