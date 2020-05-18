
#ifndef AIDE_APPLICATION_HPP
#define AIDE_APPLICATION_HPP

#include <QApplication>

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

    private:
        static bool isOrganizationNameSet();
    };
} // namespace aide
#endif // AIDE_APPLICATION_HPP
