#include "mainwindowcontroller.hpp"

#include <QCloseEvent>

#include "applicationclosecontroller.hpp"
#include "mainwindowgeometryandstatecontroller.hpp"

using aide::core::ApplicationCloseController;
using aide::core::MainWindowGeometryAndStateController;
using aide::core::ShowSettingsDialogController;
using aide::gui::MainWindowController;

MainWindowController::MainWindowController(
    const ApplicationCloseController& closeUseCase,
    MainWindowGeometryAndStateController& saveUseCase,
    ShowSettingsDialogController& settingsDialogUseCase)
    : applicationCloseInteractor(closeUseCase)
    , saveGeometryAndStateInteractor(saveUseCase)
    , showSettingsDialogInteractor(settingsDialogUseCase)
{}

void MainWindowController::onUserWantsToQuitApplication(
    QCloseEvent* event, const QByteArray& geometry, const QByteArray& state)
{
    const auto allowedToClose = applicationCloseInteractor.isCloseAllowed();

    if (allowedToClose) {
        saveGeometryAndStateInteractor.saveGeometryAndState(geometry, state);
        event->accept();
    } else {
        event->ignore();
    }
}

void MainWindowController::onUserWantsToShowSettingsDialog()
{
    showSettingsDialogInteractor.showSettingsDialog();
}
