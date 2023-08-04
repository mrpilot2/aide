

#ifndef AIDE_MENU_CONTAINER_INTERFACE_HPP
#define AIDE_MENU_CONTAINER_INTERFACE_HPP

#include <memory>

#include <QObject>
class QMenu;

namespace aide
{
    class MenuContainerInterface : public QObject
    {
        Q_OBJECT
    public:
        virtual ~MenuContainerInterface();

        [[nodiscard]] virtual QMenu* menu() const = 0;
    };

    using MenuContainerInterfacePtr = std::shared_ptr<MenuContainerInterface>;
} // namespace aide

#endif // AIDE_MENU_CONTAINER_INTERFACE_HPP
