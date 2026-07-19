
#ifndef AIDE_APPLICATION_HPP
#define AIDE_APPLICATION_HPP

#include <memory>

#include <QApplication>

#include <aide/actionregistryinterface.hpp>
#include <aide/appearancemanager.hpp>
#include <aide/applicationconfig.hpp>
#include <aide/loggerinterface.hpp>
#include <aide/mainwindowinterface.hpp>
#include <aide/notificationmanagerinterface.hpp>
#include <aide/settings/settingspageregistry.hpp>

namespace aide
{
    class ApplicationBuilder;
    class ActionRegistry;
    class AideSettingsProvider;

    namespace gui
    {
        class MainWindow;
        class TranslatorInterface;
    } // namespace gui

    /**
     * @brief The aide::Application class manages the GUI application
     *
     * This class is derived from QApplication and is the main entry point
     * of the library. Therefore it can be used in the same way as a normal
     * QApplication
     *
     * It creates and manages the main window and main settings.
     *
     */
    class Application : public QApplication
    {
    public:
        // NOLINTNEXTLINE
        explicit Application(int& argc, char* argv[]);

        // NOLINTNEXTLINE
        Application(int& argc, char* argv[], const ApplicationConfig& config);

        [[nodiscard]] static aide::LoggerPtr logger();

        [[nodiscard]] static aide::LoggerPtr logger(
            const std::string& loggerName);

        static void disableLoggingToConsole();

        [[nodiscard]] std::shared_ptr<core::MainWindowInterface> mainWindow()
            const;

        [[nodiscard]] std::shared_ptr<gui::TranslatorInterface> translator()
            const;

        [[nodiscard]] std::shared_ptr<AideSettingsProvider> settingsProvider()
            const;

        [[nodiscard]] ActionRegistryInterfacePtr actionRegistry() const;

        [[nodiscard]] NotificationManagerInterface& notificationManager() const;

        [[nodiscard]] core::SettingsPageRegistry& settingsPageRegistry() const;

        [[nodiscard]] AppearanceManager& appearanceManager() const;

        [[nodiscard]] const ApplicationConfig& config() const;

    private:
        static bool isOrganizationNameSet();

        std::shared_ptr<aide::ApplicationBuilder> m_appBuilder;
    };
} // namespace aide
#endif // AIDE_APPLICATION_HPP
