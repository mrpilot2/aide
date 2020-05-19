
#ifndef AIDE_APPLICATION_HPP
#define AIDE_APPLICATION_HPP

#include <memory>

#include <QApplication>

#include <aide/logger.hpp>

class QMainWindow;

namespace aide
{
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
        explicit Application(int argc, char* argv[]);

        [[nodiscard]] std::shared_ptr<Logger> logger() const;

    private:
        static bool isOrganizationNameSet();

        void setupLogger();

        [[nodiscard]] static bool tryToCreateLogLocationIfItDoesNotExist(
            const QString& logLocation);

        std::shared_ptr<aide::Logger> m_logger;

        std::shared_ptr<QMainWindow> m_mainWindow;
    };
} // namespace aide
#endif // AIDE_APPLICATION_HPP