#ifndef AIDE_APPLICATION_BUILDER_HPP
#define AIDE_APPLICATION_BUILDER_HPP

#include <QString>

#include <aide/logger.hpp>

#include "actionregistry.hpp"
#include "aidesettingsprovider.hpp"
#include "applicationclose.hpp"
#include "gui/mainwindow.hpp"
#include "gui/mainwindowcontroller.hpp"

namespace aide
{
    class ApplicationBuilder
    {
    public:
        ApplicationBuilder();

        [[nodiscard]] std::shared_ptr<aide::Logger> logger() const;

        [[nodiscard]] std::shared_ptr<aide::gui::MainWindow> mainWindow() const;

    private:
        static std::shared_ptr<aide::Logger> setupLogger();

        [[nodiscard]] static bool tryToCreateLogLocationIfItDoesNotExist(
            const QString& logLocation);

        std::shared_ptr<aide::Logger> m_logger{setupLogger()};

        std::shared_ptr<aide::ActionRegistry> m_actionRegistry;

        std::shared_ptr<aide::gui::MainWindow> m_mainWindow;

        AideSettingsProvider settingsProvider;

        aide::core::ApplicationClose m_applicationClose;

        aide::gui::MainWindowControllerPtr m_mainController;
    };
} // namespace aide
#endif // AIDE_APPLICATION_BUILDER_HPP
