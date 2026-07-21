#include "mainwindowcontroller.hpp"

#include <utility>

#include <QCloseEvent>

#include <aide/aideconstants.hpp>

#include "aboutaidedialog.hpp"
#include "aboutaideusecase.hpp"
#include "applicationclosecontroller.hpp"
#include "logger/loggerfactory.hpp"
#include "mainwindow.hpp"
#include "mainwindowgeometryandstatecontroller.hpp"
#include "osfilemanagerlauncher.hpp"
#include "reportbugusecase.hpp"
#include "showloginfilemanagerusecase.hpp"

using aide::constants::CONSTANTS;
using aide::core::AboutAideUseCase;
using aide::core::ApplicationCloseController;
using aide::core::MainWindowGeometryAndStateController;
using aide::core::ShowSettingsDialogController;
using aide::gui::MainWindowController;

MainWindowController::MainWindowController(
    std::shared_ptr<MainWindow> mainWindow,
    const ApplicationCloseController& closeUseCase,
    MainWindowGeometryAndStateController& saveUseCase,
    ShowSettingsDialogController& settingsDialogUseCase,
    aide::UrlLauncherPtr urlLauncher)
    : m_mainWindow(std::move(mainWindow))
    , applicationCloseInteractor(closeUseCase)
    , saveGeometryAndStateInteractor(saveUseCase)
    , showSettingsDialogInteractor(settingsDialogUseCase)
    , m_urlLauncher(std::move(urlLauncher))
{}

void MainWindowController::onUserWantsToQuitApplication(
    QCloseEvent* event, const QByteArray& geometry,
    const QByteArray& state) const
{
    if (applicationCloseInteractor.isCloseAllowed()) {
        saveGeometryAndStateInteractor.saveGeometryAndState(geometry, state);
        event->accept();
    } else {
        event->ignore();
    }
}

void MainWindowController::onUserWantsToShowSettingsDialog() const
{
    showSettingsDialogInteractor.showSettingsDialog();
}

void MainWindowController::onUserWantsToShowNotificationSettings() const
{
    showSettingsDialogInteractor.showSettingsDialog(
        CONSTANTS().SETTINGS_NOTIFICATIONS);
}

void MainWindowController::onUserWantsToShowAboutAideDialog() const
{
    const auto dialog = std::make_shared<AboutAideDialog>(m_mainWindow.get());
    const AboutAideUseCase useCase(
        dialog, core::LoggerFactory::createLogger("AboutAide"));
    useCase.showAboutAideInformation();
}

void MainWindowController::onUserWantsToShowLogInFileManager()
{
    const auto launcher = std::make_shared<core::OsFileManagerLauncher>();
    const core::ShowLogInFileManagerUseCase useCase(
        launcher, core::LoggerFactory::createLogger("ShowLogInFileManager"));
    useCase.showLogInFileManager();
}

void MainWindowController::onUserWantsToReportBug() const
{
    const core::ReportBugUseCase useCase(
        m_urlLauncher, core::LoggerFactory::createLogger("ReportBug"));
    useCase.reportBug();
}
