#include "mockmainwindowview.hpp"

using aide::test::MockMainWindowView;

void MockMainWindowView::restoreGeometryAndState(const QByteArray geometry,
                                                 const QByteArray state)
{
    currentGeometry = geometry;
    currentState    = state;
}

aide::widgets::Banner* MockMainWindowView::addBanner(NotificationType /*type*/,
                                                     const QString& /*message*/)
{
    return nullptr;
}

void MockMainWindowView::removeBanner(aide::widgets::Banner* /*banner*/) {}

const QByteArray& MockMainWindowView::getState() const
{
    return currentState;
}

const QByteArray& MockMainWindowView::getGeometry() const
{
    return currentGeometry;
}
