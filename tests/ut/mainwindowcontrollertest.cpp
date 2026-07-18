#include <memory>

#include <catch2/catch_test_macros.hpp>

#include <QApplication>
#include <QByteArray>
#include <QCloseEvent>

#include <aide/applicationconfig.hpp>

#include "applicationclosecontroller.hpp"
#include "mainwindow.hpp"
#include "mainwindowcontroller.hpp"
#include "mainwindowgeometryandstatecontroller.hpp"
#include "mockurllauncher.hpp"
#include "nulllogger.hpp"
#include "settings/showsettingsdialogcontroller.hpp"

using aide::ApplicationConfig;
using aide::core::ApplicationCloseController;
using aide::core::MainWindowGeometryAndStateController;
using aide::core::ShowSettingsDialogController;
using aide::gui::MainWindow;
using aide::gui::MainWindowController;
using aide::test::NullLogger;
using aide::tests::MockUrlLauncher;

namespace
{
    class FakeApplicationCloseController : public ApplicationCloseController
    {
    public:
        bool allowClose{true};

        [[nodiscard]] bool isCloseAllowed() const override
        {
            return allowClose;
        }
    };

    class FakeMainWindowGeometryAndStateController
        : public MainWindowGeometryAndStateController
    {
    public:
        bool saveCalled{false};
        QByteArray lastGeometry;
        QByteArray lastState;

        void saveGeometryAndState(const QByteArray& geometry,
                                  const QByteArray& state) override
        {
            saveCalled   = true;
            lastGeometry = geometry;
            lastState    = state;
        }

        void restoreGeometryAndState() override {}
    };

    class FakeShowSettingsDialogController : public ShowSettingsDialogController
    {
    public:
        bool showCalled{false};

        void showSettingsDialog() override { showCalled = true; }
    };
} // namespace

TEST_CASE("A main window controller handling a close request")
{
    QApplication::setApplicationName("aide_test");
    QApplication::setOrganizationName("aide_company");

    const auto mainWindow = std::make_shared<MainWindow>(
        std::make_shared<NullLogger>(), ApplicationConfig{}, nullptr);

    FakeApplicationCloseController closeController;
    FakeMainWindowGeometryAndStateController saveController;
    FakeShowSettingsDialogController settingsController;

    const MainWindowController controller(mainWindow, closeController,
                                          saveController, settingsController,
                                          std::make_shared<MockUrlLauncher>());

    const QByteArray geometry("geometry-bytes");
    const QByteArray state("state-bytes");

    SECTION("accepts the close event and saves geometry when close is allowed")
    {
        closeController.allowClose = true;

        QCloseEvent event;
        controller.onUserWantsToQuitApplication(&event, geometry, state);

        REQUIRE(event.isAccepted());
        REQUIRE(saveController.saveCalled);
        REQUIRE(saveController.lastGeometry == geometry);
        REQUIRE(saveController.lastState == state);
    }

    SECTION(
        "ignores the close event and does not save geometry when close is "
        "not allowed")
    {
        closeController.allowClose = false;

        QCloseEvent event;
        controller.onUserWantsToQuitApplication(&event, geometry, state);

        REQUIRE_FALSE(event.isAccepted());
        REQUIRE_FALSE(saveController.saveCalled);
    }
}

TEST_CASE(
    "A main window controller handling a request to show the settings "
    "dialog")
{
    QApplication::setApplicationName("aide_test");
    QApplication::setOrganizationName("aide_company");

    const auto mainWindow = std::make_shared<MainWindow>(
        std::make_shared<NullLogger>(), ApplicationConfig{}, nullptr);

    const FakeApplicationCloseController closeController;
    FakeMainWindowGeometryAndStateController saveController;
    FakeShowSettingsDialogController settingsController;

    const MainWindowController controller(mainWindow, closeController,
                                          saveController, settingsController,
                                          std::make_shared<MockUrlLauncher>());

    SECTION("delegates to the show settings dialog interactor")
    {
        controller.onUserWantsToShowSettingsDialog();

        REQUIRE(settingsController.showCalled);
    }
}

TEST_CASE("A main window controller handling a request to report a bug")
{
    QApplication::setApplicationName("aide_test");
    QApplication::setOrganizationName("aide_company");

    const auto mainWindow = std::make_shared<MainWindow>(
        std::make_shared<NullLogger>(), ApplicationConfig{}, nullptr);

    const FakeApplicationCloseController closeController;
    FakeMainWindowGeometryAndStateController saveController;
    FakeShowSettingsDialogController settingsController;

    const auto urlLauncher = std::make_shared<MockUrlLauncher>();

    const MainWindowController controller(mainWindow, closeController,
                                          saveController, settingsController,
                                          urlLauncher);

    SECTION(
        "asks the launcher supplied at construction time to open the bug "
        "report URL, instead of the default OS-backed launcher")
    {
        controller.onUserWantsToReportBug();

        REQUIRE(urlLauncher->lastRequestedUrl.has_value());
    }
}
