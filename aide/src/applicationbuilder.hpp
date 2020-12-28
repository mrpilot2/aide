#ifndef AIDE_APPLICATION_BUILDER_HPP
#define AIDE_APPLICATION_BUILDER_HPP

#include <QString>

#include "actionregistry.hpp"
#include "aidesettingsprovider.hpp"
#include "applicationclose.hpp"
#include "gui/mainwindow.hpp"
#include "gui/mainwindowcontroller.hpp"
#include "loggerinterface.hpp"
#include "mainwindowgeometryandstate.hpp"

namespace aide
{
    class ApplicationBuilder
    {
    public:
        ApplicationBuilder();

        [[nodiscard]] std::shared_ptr<aide::LoggerInterface> logger() const;

        [[nodiscard]] std::shared_ptr<aide::gui::MainWindow> mainWindow() const;

    private:
        static aide::LoggerPtr setupLogger();

        [[nodiscard]] static bool tryToCreateLogLocationIfItDoesNotExist(
            const QString& logLocation);

        aide::LoggerPtr m_logger{setupLogger()};

        std::shared_ptr<aide::ActionRegistry> m_actionRegistry;

        std::shared_ptr<aide::gui::MainWindow> m_mainWindow;

        AideSettingsProvider settingsProvider;

        aide::core::ApplicationClose m_applicationClose;

        aide::core::MainWindowGeometryAndState m_mainWindowGeometryAndState;

        aide::gui::MainWindowControllerPtr m_mainController;
    };
} // namespace aide
#endif // AIDE_APPLICATION_BUILDER_HPP
