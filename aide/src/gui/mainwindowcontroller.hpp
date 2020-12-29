
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
    class MainWindowController : public QObject
    {
        Q_OBJECT
    public:
        explicit MainWindowController(
            const aide::core::ApplicationCloseController& closeUseCase,
            aide::core::MainWindowGeometryAndStateController& saveUseCase,
            const aide::core::ShowSettingsDialogController&
                settingsDialogUseCase);

        void onUserWantsToQuitApplication(QCloseEvent* event,
                                          const QByteArray& geometry,
                                          const QByteArray& state);

    public slots:
        void onUserWantsToShowSettingsDialog() const;

    private:
        const aide::core::ApplicationCloseController&
            applicationCloseInteractor;
        aide::core::MainWindowGeometryAndStateController&
            saveGeometryAndStateInteractor;

        const aide::core::ShowSettingsDialogController&
            showSettingsDialogInteractor;
    };

    using MainWindowControllerPtr = std::shared_ptr<MainWindowController>;
} // namespace aide::gui

#endif // AIDE_MAIN_WINDOW_CONTROLLER_HPP
