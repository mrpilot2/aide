
#ifndef AIDE_APPLICATION_HPP
#define AIDE_APPLICATION_HPP

#include <memory>

#include <QApplication>

#include <aide/actionregistryinterface.hpp>
#include <aide/applicationconfig.hpp>
#include <aide/loggerinterface.hpp>

class QMainWindow;

namespace aide
{
    class ApplicationBuilder;
    class ActionRegistry;
    class AideSettingsProvider;
    class SentryCleanup;

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
        explicit Application(int& argc, char* argv[],
                             const ApplicationConfig& config);

        ~Application();

        [[nodiscard]] static aide::LoggerPtr logger();

        [[nodiscard]] static aide::LoggerPtr logger(
            const std::string& loggerName);

        [[nodiscard]] std::shared_ptr<QMainWindow> mainWindow() const;

        [[nodiscard]] std::shared_ptr<gui::TranslatorInterface> translator()
            const;

        [[nodiscard]] std::shared_ptr<AideSettingsProvider> settingsProvider();

        [[nodiscard]] ActionRegistryInterfacePtr actionRegistry() const;

    private:
        static bool isOrganizationNameSet();

        std::shared_ptr<aide::ApplicationBuilder> m_appBuilder;

        [[maybe_unused]] std::unique_ptr<SentryCleanup> m_sentryCleanup;
    };
} // namespace aide
#endif // AIDE_APPLICATION_HPP
