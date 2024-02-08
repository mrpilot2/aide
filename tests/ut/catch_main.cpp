#define CATCH_CONFIG_RUNNER
#include <catch2/catch.hpp>

#include <aide/aidesettingsprovider.hpp>

#include "mocksettings.hpp"

int main(int argc, char* argv[])
{
    aide::AideSettingsProvider::provideVersionableSettings(
        std::make_shared<aide::test::MockSettings>());
    aide::AideSettingsProvider::provideUnVersionableSettings(
        std::make_shared<aide::test::MockSettings>());

    const int result = Catch::Session().run(argc, argv);

    return result;
}
