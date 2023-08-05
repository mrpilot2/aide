

#ifndef AIDE_MENU_CONTAINER_HPP
#define AIDE_MENU_CONTAINER_HPP

#include <memory>

#include <QMenu>

#include "menucontainerinterface.hpp"

class QWidget;

namespace aide
{
    class MenuContainer : public MenuContainerInterface
    {
        Q_OBJECT
    public:
        explicit MenuContainer(QWidget* parent = nullptr);

        ~MenuContainer() override;

        [[nodiscard]] QMenu* menu() const override;

    private:
        QMenu* m_menu;
    };
} // namespace aide

#endif // AIDE_MENU_CONTAINER_HPP
