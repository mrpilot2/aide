#define CATCH_CONFIG_RUNNER
#include <catch2/catch_session.hpp>

#include <aide/aidesettingsprovider.hpp>

#include "mocksettings.hpp"

int main(int argc, char* argv[])
{
    aide::AideSettingsProvider::provideVersionableSettings(
        std::make_shared<aide::test::MockSettings>());
    aide::AideSettingsProvider::provideUnVersionableSettings(
        std::make_shared<aide::test::MockSettings>());

    // Deliberately no QApplication here: aide::Application is itself a
    // QApplication and the [Application] tests construct one under test. A
    // process-global QApplication would collide with it, so these tests live in
    // their own executable that pre-creates nothing.
    return Catch::Session().run(argc, argv);
}
