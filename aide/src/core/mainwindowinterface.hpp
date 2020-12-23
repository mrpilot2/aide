#ifndef AIDE_MAIN_WINDOW_INTERFACE_HPP
#define AIDE_MAIN_WINDOW_INTERFACE_HPP

#include <QMainWindow>

namespace aide::core
{
    class MainWindowInterface : public QMainWindow
    {
    public:
        explicit MainWindowInterface(QWidget* parent = nullptr);

        virtual ~MainWindowInterface() = default;
    };
} // namespace aide::core

#endif // AIDE_MAIN_WINDOW_INTERFACE_HPP
