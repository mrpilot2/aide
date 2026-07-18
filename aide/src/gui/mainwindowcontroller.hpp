
#ifndef AIDE_MAIN_WINDOW_CONTROLLER_HPP
#define AIDE_MAIN_WINDOW_CONTROLLER_HPP

#include <memory>

#include <QObject>

#include <settings/showsettingsdialogcontroller.hpp>

class QByteArray;
class QCloseEvent;

namespace aide::core
{
    class ApplicationCloseController;
    class MainWindowGeometryAndStateController;
} // namespace aide::core

namespace aide::gui
{
    class MainWindow;

    class MainWindowController : public QObject
    {
        Q_OBJECT
    public:
        explicit MainWindowController(
            std::shared_ptr<MainWindow> mainWindow,
            const aide::core::ApplicationCloseController& closeUseCase,
            aide::core::MainWindowGeometryAndStateController& saveUseCase,
            core::ShowSettingsDialogController& settingsDialogUseCase);

        void onUserWantsToQuitApplication(QCloseEvent* event,
                                          const QByteArray& geometry,
                                          const QByteArray& state) const;

        // Not a member of the instance's state, so it is a plain static
        // method rather than an instance slot; it is still connected via
        // the new-style pointer-based connect() with the controller
        // instance supplied as the context object.
        static void onUserWantsToShowLogInFileManager();

        // Not a member of the instance's state either; see the comment on
        // onUserWantsToShowLogInFileManager above.
        static void onUserWantsToReportBug();

    public slots:
        void onUserWantsToShowSettingsDialog() const;

        void onUserWantsToShowAboutAideDialog() const;

    private:
        std::shared_ptr<MainWindow> m_mainWindow;

        const aide::core::ApplicationCloseController&
            applicationCloseInteractor;
        aide::core::MainWindowGeometryAndStateController&
            saveGeometryAndStateInteractor;

        aide::core::ShowSettingsDialogController& showSettingsDialogInteractor;
    };

    using MainWindowControllerPtr = std::shared_ptr<MainWindowController>;
} // namespace aide::gui

#endif // AIDE_MAIN_WINDOW_CONTROLLER_HPP
