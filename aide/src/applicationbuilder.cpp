#include "applicationbuilder.hpp"

#include <utility>

#include <settings/keymap/keymappage.hpp>

#include "core/urllauncherresolver.hpp"
#include "gui/settings/appearancepage.hpp"
#include "gui/settings/notifications/notificationssettingspage.hpp"
#include "logger/loggerfactory.hpp"

using aide::ApplicationBuilder;
using aide::LoggerPtr;
using aide::core::KeymapPage;
using aide::core::UrlLauncherResolver;
using aide::gui::KeymapPageWidget;
using aide::gui::MainWindow;
using aide::gui::MainWindowController;
using aide::gui::SettingsDialog;
using aide::gui::SettingsDialogController;

ApplicationBuilder::ApplicationBuilder(ApplicationConfig config)
    : m_config(std::move(config))
    , m_settingsProvider(std::make_shared<AideSettingsProvider>())
    , m_actionRegistry{std::make_shared<ActionRegistry>(
          *(AideSettingsProvider::versionableSettings()), m_logger)}
    , m_notificationManager{std::make_shared<NotificationManager>(
          *(AideSettingsProvider::versionableSettings()))}
    , m_mainWindow(new MainWindow(m_logger, m_config, nullptr))
    , m_notificationBalloonHost(std::make_unique<gui::NotificationBalloonHost>(
          *m_notificationManager,
          *(AideSettingsProvider::versionableSettings()), m_mainWindow.get()))
    , m_settingsDialog(std::make_shared<SettingsDialog>(m_mainWindow.get()))
    , m_applicationClose(m_mainWindow,
                         *(AideSettingsProvider::versionableSettings()))
    , m_mainWindowGeometryAndState(
          m_mainWindow, *(AideSettingsProvider::unversionableSettings()))
    , m_showSettingsDialog(m_settingsDialog, m_settingsPageRegistry,
                           *(AideSettingsProvider::unversionableSettings()),
                           m_logger)
    , m_settingsDialogController(
          std::make_shared<SettingsDialogController>(m_showSettingsDialog))
    , m_mainController(std::make_shared<MainWindowController>(
          m_mainWindow, m_applicationClose, m_mainWindowGeometryAndState,
          m_showSettingsDialog, UrlLauncherResolver::resolve(m_config)))
    , m_keyMapPageWidget{std::make_unique<KeymapPageWidget>(
          m_settingsDialog.get())}
    , m_keyMapPage(std::make_shared<KeymapPage>(m_actionRegistry,
                                                m_keyMapPageWidget.get()))
    , m_keymapPageController(std::make_shared<gui::KeyMapPageWidgetController>(
          m_keyMapPage->getTreeModel(), m_keyMapPage->keyMapWidget()))
{
    m_mainWindow->setMainWindowController(m_mainController, m_actionRegistry);
    m_mainWindow->createNotificationLogView(
        *m_notificationManager, *(AideSettingsProvider::versionableSettings()));
    m_settingsDialog->setController(m_settingsDialogController);

    m_mainWindowGeometryAndState.restoreGeometryAndState();

    if (auto* widget =
            dynamic_cast<KeymapPageWidget*>(m_keyMapPage->keyMapWidget());
        widget != nullptr) {
        widget->setController(m_keymapPageController);
    }

    m_settingsPageRegistry.addPage(
        std::make_shared<gui::AppearancePage>(m_appearanceManager));
    m_settingsPageRegistry.addPage(m_keyMapPage);
    m_settingsPageRegistry.addPage(
        std::make_shared<gui::NotificationsSettingsPage>(
            *m_notificationManager,
            *(AideSettingsProvider::versionableSettings())));

    // String-based connect on purpose: a pointer-to-member connect references
    // the sender's and receiver's staticMetaObject data symbols, which are not
    // exported across DLL boundaries on MSVC (CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS
    // exports functions, not data). The string form resolves the signal/slot
    // at runtime via the exported virtual metaObject(), so it links there.
    QObject::connect(&m_appearanceManager, SIGNAL(appearanceChanged()),
                     m_mainWindow.get(), SLOT(refreshIcons()));

    // Same string-based-connect reasoning as above: NotificationManager
    // (AideCore) is the sender, NotificationBalloonHost (AideGui) the
    // receiver, so a pointer-to-member connect would not link on MSVC.
    QObject::connect(m_notificationManager.get(),
                     SIGNAL(notificationPosted(aide::NotificationId)),
                     m_notificationBalloonHost.get(),
                     SLOT(onNotificationPosted(aide::NotificationId)));
}

aide::AppearanceManager& ApplicationBuilder::appearanceManager()
{
    return m_appearanceManager;
}

const aide::ApplicationConfig& ApplicationBuilder::config() const
{
    return m_config;
}

LoggerPtr ApplicationBuilder::logger() const
{
    return m_logger;
}

LoggerPtr ApplicationBuilder::setupLogger(const std::string& loggerName)
{
    return core::LoggerFactory::createLogger(loggerName);
}

LoggerPtr ApplicationBuilder::setupLogger()
{
    return core::LoggerFactory::createLogger();
}

std::shared_ptr<MainWindow> ApplicationBuilder::mainWindow() const
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

aide::NotificationManagerInterface& ApplicationBuilder::notificationManager()
    const
{
    return *m_notificationManager;
}

aide::core::SettingsPageRegistry& ApplicationBuilder::settingsPageRegistry()
{
    return m_settingsPageRegistry;
}
