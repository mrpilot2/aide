#include "applicationbuilder.hpp"

#include <iostream>

#include <QApplication>
#include <QDir>
#include <QStandardPaths>
#include <QString>

#include "logger/logger.hpp"

using aide::ApplicationBuilder;
using aide::Logger;
using aide::LoggerPtr;
using aide::gui::MainWindow;
using aide::gui::MainWindowController;
using aide::gui::SettingsDialog;
using aide::gui::SettingsDialogController;

ApplicationBuilder::ApplicationBuilder()
    : m_actionRegistry{std::make_shared<ActionRegistry>(m_logger)}
    , m_mainWindow(new MainWindow(m_logger, nullptr))
    , m_settingsDialog(std::make_shared<SettingsDialog>(m_mainWindow.get()))
    , m_settingsProvider(std::make_shared<AideSettingsProvider>())
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
          m_applicationClose, m_mainWindowGeometryAndState,
          m_showSettingsDialog))
{
    m_mainWindow->setMainWindowController(m_mainController, m_actionRegistry);
    m_settingsDialog->setController(m_settingsDialogController);

    m_mainWindowGeometryAndState.restoreGeometryAndState();
}

LoggerPtr ApplicationBuilder::logger() const
{
    return m_logger;
}

LoggerPtr ApplicationBuilder::setupLogger()
{
    QString logLocation(
        QStandardPaths::writableLocation(QStandardPaths::CacheLocation));

    bool success = tryToCreateLogLocationIfItDoesNotExist(logLocation);

    if (!success) {
        logLocation =
            QStandardPaths::writableLocation(QStandardPaths::TempLocation);
        success = tryToCreateLogLocationIfItDoesNotExist(logLocation);
    }

    if (success) {
        FileName logPath(logLocation.append("/")
                             .append(QApplication::applicationName())
                             .append(".log")
                             .toStdString());

        return std::make_shared<aide::Logger>(logPath);
    }
    return std::make_shared<Logger>();
}

bool ApplicationBuilder::tryToCreateLogLocationIfItDoesNotExist(
    const QString& logLocation)
{
    if (!logLocation.isEmpty()) {
        QDir d;
        if (!d.mkpath(logLocation)) {
            std::cerr << "Could not create standard log directory: "
                      << logLocation.toStdString()
                      << ". Trying to log into temp directory instead. If this "
                         "also does not work no file logging will happen.";
            return false;
        }
        return true;
    }
    return false;
}

std::shared_ptr<aide::gui::MainWindow> ApplicationBuilder::mainWindow() const
{
    return m_mainWindow;
}

std::shared_ptr<aide::SettingsProviderInterface>
ApplicationBuilder::settingsProvider() const
{
    return m_settingsProvider;
}

aide::ActionRegistryInterfacePtr ApplicationBuilder::actionRegistry() const
{
    return m_actionRegistry;
}
