#define CATCH_CONFIG_RUNNER
#include <array>
#include <memory>

#include <catch2/catch_session.hpp>
#include <catch2/reporters/catch_reporter_event_listener.hpp>
#include <catch2/reporters/catch_reporter_registrars.hpp>

#include <QApplication>

#include <aide/aidesettingsprovider.hpp>

#include "mocksettings.hpp"

namespace
{
    // Every widget-based test relies on a live QApplication. Create exactly one
    // for the whole process, but only once the tests actually start running:
    // catch_discover_tests enumerates the test cases at build time by running
    // the binary with --list-tests, and that must not require a display.
    // Listing never starts a test run, so constructing the QApplication here
    // keeps discovery headless while still giving every run a single
    // application that outlives all the widgets its tests create.
    class QApplicationLifetime : public Catch::EventListenerBase
    {
    public:
        using Catch::EventListenerBase::EventListenerBase;

        void testRunStarting(const Catch::TestRunInfo& /*info*/) override
        {
            // argc/argv must outlive the QApplication, hence static storage.
            static int argc{1};
            // NOLINTNEXTLINE
            static std::array<char*, 1> argv{{const_cast<char*>("aide_test")}};
            app = std::make_unique<QApplication>(argc, argv.data());
        }

        void testRunEnded(const Catch::TestRunStats& /*stats*/) override
        {
            app.reset();
        }

    private:
        std::unique_ptr<QApplication> app;
    };
} // namespace

// cppcheck-suppress unknownMacro
CATCH_REGISTER_LISTENER(QApplicationLifetime)

int main(int argc, char* argv[])
{
    // Widget tests never need a real compositor, and the Wayland QPA plugin
    // refuses to grab popups (QCompleter, QMenu, ...) for a window that
    // hasn't received real input focus from the compositor - something an
    // automated test run never gets on a host with a live desktop session.
    // Default to offscreen, matching CI, but let an explicit override win.
    if (!qEnvironmentVariableIsSet("QT_QPA_PLATFORM")) {
        qputenv("QT_QPA_PLATFORM", "offscreen");
    }

    aide::AideSettingsProvider::provideVersionableSettings(
        std::make_shared<aide::test::MockSettings>());
    aide::AideSettingsProvider::provideUnVersionableSettings(
        std::make_shared<aide::test::MockSettings>());

    return Catch::Session().run(argc, argv);
}
