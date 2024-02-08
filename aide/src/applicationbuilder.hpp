#ifndef AIDE_APPLICATION_BUILDER_HPP
#define AIDE_APPLICATION_BUILDER_HPP

#include <settings/keymap/keymappage.hpp>

#include "actionregistry.hpp"
#include "aidesettingsprovider.hpp"
#include "applicationclose.hpp"
#include "gui/mainwindow.hpp"
#include "gui/mainwindowcontroller.hpp"
#include "gui/settings/keymap/keymappagewidget.hpp"
#include "gui/settings/settingsdialog.hpp"
#include "loggerinterface.hpp"
#include "mainwindowgeometryandstate.hpp"
#include "settings/keymap/keymappagewidgetcontroller.hpp"
#include "settings/showsettingsdialog.hpp"

namespace aide
{
    class ApplicationBuilder
    {
    public:
        ApplicationBuilder();

        [[nodiscard]] std::shared_ptr<aide::LoggerInterface> logger() const;

        [[nodiscard]] std::shared_ptr<aide::gui::MainWindow> mainWindow() const;

        [[nodiscard]] std::shared_ptr<AideSettingsProvider> settingsProvider()
            const;

        [[nodiscard]] ActionRegistryInterfacePtr actionRegistry() const;

        static LoggerPtr setupLogger(const std::string& loggerName);

    private:
        // cppcheck-suppress unusedPrivateFunction
        static LoggerPtr setupLogger();

        aide::LoggerPtr m_logger{ApplicationBuilder::setupLogger()};

        std::shared_ptr<AideSettingsProvider> m_settingsProvider;

        std::shared_ptr<aide::ActionRegistry> m_actionRegistry;

        std::shared_ptr<aide::gui::MainWindow> m_mainWindow;

        std::shared_ptr<aide::gui::SettingsDialog> m_settingsDialog;

        aide::core::ApplicationClose m_applicationClose;

        aide::core::MainWindowGeometryAndState m_mainWindowGeometryAndState;

        aide::core::ShowSettingsDialog m_showSettingsDialog;

        aide::gui::SettingsDialogControllerPtr m_settingsDialogController;

        aide::gui::MainWindowControllerPtr m_mainController;

        std::unique_ptr<aide::gui::KeymapPageWidget> m_keyMapPageWidget;

        std::shared_ptr<core::KeymapPage> m_keyMapPage;

        aide::gui::KeyMapPageControllerPtr m_keymapPageController;
    };
} // namespace aide
#endif // AIDE_APPLICATION_BUILDER_HPP
