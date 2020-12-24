#ifndef AIDE_MAIN_WINDOW_INTERFACE_HPP
#define AIDE_MAIN_WINDOW_INTERFACE_HPP

#include <memory>

#include <QMainWindow>

namespace aide::core
{
    class MainWindowInterface : public QMainWindow
    {
    public:
        explicit MainWindowInterface(QWidget* parent = nullptr);

        virtual ~MainWindowInterface() = default;

        virtual void restoreGeometryAndState(QByteArray geometry,
                                             QByteArray state) = 0;
    };

    using MainWindowInterfaceWeakPtr = std::weak_ptr<MainWindowInterface>;
} // namespace aide::core

#endif // AIDE_MAIN_WINDOW_INTERFACE_HPP
