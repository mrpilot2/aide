#include "mainwindowcontroller.hpp"

#include <utility>

#include <QCloseEvent>

#include "aboutaidedialog.hpp"
#include "aboutaideusecase.hpp"
#include "applicationclosecontroller.hpp"
#include "loggerfactory.hpp"
#include "mainwindow.hpp"
#include "mainwindowgeometryandstatecontroller.hpp"

using aide::core::AboutAideUseCase;
using aide::core::ApplicationCloseController;
using aide::core::MainWindowGeometryAndStateController;
using aide::core::ShowSettingsDialogController;
using aide::gui::MainWindowController;

MainWindowController::MainWindowController(
    std::shared_ptr<MainWindow> mainWindow,
    const ApplicationCloseController& closeUseCase,
    MainWindowGeometryAndStateController& saveUseCase,
    ShowSettingsDialogController& settingsDialogUseCase)
    : m_mainWindow(std::move(mainWindow))
    , applicationCloseInteractor(closeUseCase)
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

void MainWindowController::onUserWantsToShowAboutAideDialog()
{
    auto dialog = std::make_shared<AboutAideDialog>(m_mainWindow.get());
    AboutAideUseCase useCase(dialog, core::LoggerFactory::createLogger("AboutAide"));
    useCase.showAboutAideInformation();
}
