#include "applicationbuilder.hpp"

#include <QApplication>

#include <settings/keymap/keymappage.hpp>
#include <settings/settingspageregistry.hpp>

#include "logger/logger.hpp"
#include "loggerfactory.hpp"

using aide::ApplicationBuilder;
using aide::LoggerPtr;
using aide::core::KeymapPage;
using aide::gui::KeymapPageWidget;
using aide::gui::MainWindow;
using aide::gui::MainWindowController;
using aide::gui::SettingsDialog;
using aide::gui::SettingsDialogController;

ApplicationBuilder::ApplicationBuilder()
    : m_settingsProvider(std::make_shared<AideSettingsProvider>())
    , m_actionRegistry{std::make_shared<ActionRegistry>(
          *(m_settingsProvider->versionableSettings()), m_logger)}
    , m_mainWindow(new MainWindow(m_logger, nullptr))
    , m_settingsDialog(std::make_shared<SettingsDialog>(m_mainWindow.get()))
    , m_applicationClose(m_mainWindow,
                         *(m_settingsProvider->versionableSettings()))
    , m_mainWindowGeometryAndState(
          m_mainWindow, *(m_settingsProvider->unversionableSettings()))
    , m_showSettingsDialog(m_settingsDialog,
                           *(m_settingsProvider->unversionableSettings()),
                           m_logger)
    , m_settingsDialogController(
          std::make_shared<SettingsDialogController>(m_showSettingsDialog))
    , m_mainController(std::make_shared<MainWindowController>(
          m_mainWindow, m_applicationClose, m_mainWindowGeometryAndState,
          m_showSettingsDialog))
    , m_keyMapPageWidget{std::make_unique<KeymapPageWidget>(
          m_settingsDialog.get())}
    , m_keyMapPage(std::make_shared<KeymapPage>(m_actionRegistry,
                                                m_keyMapPageWidget.get()))
    , m_keymapPageController(std::make_shared<gui::KeyMapPageWidgetController>(
          m_keyMapPage->getTreeModel(), m_keyMapPage->keyMapWidget()))
{
    aide::core::SettingsPageRegistry::deleteAllPages();

    m_mainWindow->setMainWindowController(m_mainController, m_actionRegistry);
    m_settingsDialog->setController(m_settingsDialogController);

    m_mainWindowGeometryAndState.restoreGeometryAndState();

    if (auto* widget = dynamic_cast<aide::gui::KeymapPageWidget*>(
            m_keyMapPage->keyMapWidget());
        widget != nullptr) {
        widget->setController(m_keymapPageController);
    }

    aide::core::SettingsPageRegistry::addPage(m_keyMapPage);
}

LoggerPtr ApplicationBuilder::logger() const
{
    return m_logger;
}

LoggerPtr ApplicationBuilder::setupLogger(const std::string& loggerName)
{
    return aide::core::LoggerFactory::createLogger(loggerName);
}

LoggerPtr ApplicationBuilder::setupLogger()
{
    return aide::core::LoggerFactory::createLogger();
}

std::shared_ptr<aide::gui::MainWindow> ApplicationBuilder::mainWindow() const
{
    return m_mainWindow;
}

std::shared_ptr<aide::AideSettingsProvider>
ApplicationBuilder::settingsProvider() const
{
    return m_settingsProvider;
}

aide::ActionRegistryInterfacePtr ApplicationBuilder::actionRegistry() const
{
    return m_actionRegistry;
}
