
#ifndef AIDE_APPLICATION_HPP
#define AIDE_APPLICATION_HPP

#include <memory>

#include <QApplication>

#include <aide/actionregistryinterface.hpp>
#include <aide/appearancemanager.hpp>
#include <aide/applicationconfig.hpp>
#include <aide/helptopicid.hpp>
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

        /**
         * @brief Opens the Help Page of a Help Topic in the browser.
         *
         * The topic resolves to `<Help Base URL>/<topic-id>/` after any
         * remap from the ApplicationConfig, and is opened through the
         * config's UrlLauncherInterface (the OS launcher if none was set).
         * @return false, launching nothing, when isHelpAvailable() is false;
         *         otherwise whether the launcher reported success.
         */
        bool showHelp(const HelpTopicId& id);

        /**
         * @brief Whether showHelp() would open a page for @p id.
         * @return true when help is active (Feature::Help enabled and a Help
         *         Base URL set) and the topic is non-empty and not disabled.
         */
        [[nodiscard]] bool isHelpAvailable(const HelpTopicId& id) const;

        /**
         * @brief Opens Help Contents, the page at the Help Base URL itself.
         * @return false, launching nothing, when help is inactive; otherwise
         *         whether the launcher reported success.
         */
        bool showHelpContents();

    private:
        static bool isOrganizationNameSet();

        std::shared_ptr<aide::ApplicationBuilder> m_appBuilder;
    };
} // namespace aide
#endif // AIDE_APPLICATION_HPP
