
#include "menucontainer.hpp"

using aide::MenuContainer;

MenuContainer::MenuContainer(QWidget* parent)
    : m_menu{new QMenu(parent)}
{}

QMenu* aide::MenuContainer::menu() const
{
    return m_menu;
}
