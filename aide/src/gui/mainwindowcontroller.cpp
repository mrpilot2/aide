#include "mainwindowcontroller.hpp"

#include <QCloseEvent>

#include "applicationclosecontroller.hpp"
#include "mainwindowgeometryandstatecontroller.hpp"

using aide::core::ApplicationCloseController;
using aide::core::MainWindowGeometryAndStateController;
using aide::gui::MainWindowController;

MainWindowController::MainWindowController(
    const ApplicationCloseController& closeUseCase,
    MainWindowGeometryAndStateController& saveUseCase)
    : applicationCloseInteractor(closeUseCase)
    , saveGeometryAndStateInteractor(saveUseCase)
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
