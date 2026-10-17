#ifndef AIDE_FAKE_MAIN_WINDOW_CONTROLLERS_HPP
#define AIDE_FAKE_MAIN_WINDOW_CONTROLLERS_HPP

#include <memory>

#include <QByteArray>

#include <aide/hierarchicalid.hpp>

#include "applicationclosecontroller.hpp"
#include "mainwindowcontroller.hpp"
#include "mainwindowgeometryandstatecontroller.hpp"
#include "mockurllauncher.hpp"
#include "settings/showsettingsdialogcontroller.hpp"

namespace aide::test
{
    class FakeApplicationCloseController
        : public aide::core::ApplicationCloseController
    {
    public:
        bool allowClose{true};

        [[nodiscard]] bool isCloseAllowed() const override
        {
            return allowClose;
        }
    };

    class FakeMainWindowGeometryAndStateController
        : public aide::core::MainWindowGeometryAndStateController
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

    class FakeShowSettingsDialogController
        : public aide::core::ShowSettingsDialogController
    {
    public:
        bool showCalled{false};
        bool showWithSelectedGroupCalled{false};

        void showSettingsDialog() override { showCalled = true; }

        void showSettingsDialog(
            const aide::HierarchicalId& /*selectedGroup*/) override
        {
            showWithSelectedGroupCalled = true;
        }
    };

    // A real MainWindowController backed by the fakes above, for tests that
    // exercise MainWindow itself. MainWindow wires its actions to the
    // controller, so handing it nullptr instead makes every connect() fail
    // with a Qt warning and leaves the actions dangling. Declare it before the
    // MainWindow so the fakes the controller references outlive the window.
    struct FakeMainWindowControllerSet
    {
        FakeApplicationCloseController closeController;
        FakeMainWindowGeometryAndStateController geometryAndStateController;
        FakeShowSettingsDialogController settingsDialogController;
        aide::gui::MainWindowControllerPtr controller{
            std::make_shared<aide::gui::MainWindowController>(
                nullptr, closeController, geometryAndStateController,
                settingsDialogController,
                std::make_shared<aide::tests::MockUrlLauncher>())};
    };
} // namespace aide::test

#endif // AIDE_FAKE_MAIN_WINDOW_CONTROLLERS_HPP
