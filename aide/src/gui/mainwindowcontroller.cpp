#include "mainwindowcontroller.hpp"

#include <QCloseEvent>

#include "applicationclosecontroller.hpp"

using aide::gui::MainWindowController;

MainWindowController::MainWindowController(
    const aide::core::ApplicationCloseController& useCase)
    : applicationCloseInteractor(useCase)
{}

void MainWindowController::onUserWantsToQuitApplication(
    QCloseEvent* event) const
{
    applicationCloseInteractor.isCloseAllowed() ? event->accept()
                                                : event->ignore();
}
