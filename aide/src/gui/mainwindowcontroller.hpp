
#ifndef AIDE_MAIN_WINDOW_CONTROLLER_HPP
#define AIDE_MAIN_WINDOW_CONTROLLER_HPP

#include <memory>

class QCloseEvent;

namespace aide::core
{
    class ApplicationCloseController;
} // namespace aide::core

namespace aide::gui
{
    class MainWindowController
    {
    public:
        explicit MainWindowController(
            const aide::core::ApplicationCloseController& useCase);

        void onUserWantsToQuitApplication(QCloseEvent* event) const;

    private:
        const aide::core::ApplicationCloseController&
            applicationCloseInteractor;
    };

    using MainWindowControllerPtr = std::shared_ptr<MainWindowController>;
} // namespace aide::gui

#endif // AIDE_MAIN_WINDOW_CONTROLLER_HPP
