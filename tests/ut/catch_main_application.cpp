#define CATCH_CONFIG_RUNNER
#include <catch2/catch_session.hpp>

#include <QtGlobal>

#include <aide/aidesettingsprovider.hpp>

#include "mocksettings.hpp"

int main(int argc, char* argv[])
{
    // See catch_main.cpp: default to offscreen so the QApplication each
    // [Application] test constructs doesn't depend on a real compositor,
    // but let an explicit override win.
    if (!qEnvironmentVariableIsSet("QT_QPA_PLATFORM")) {
        qputenv("QT_QPA_PLATFORM", "offscreen");
    }

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
