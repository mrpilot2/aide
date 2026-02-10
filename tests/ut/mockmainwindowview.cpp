#include "mockmainwindowview.hpp"

using aide::test::MockMainWindowView;

void MockMainWindowView::restoreGeometryAndState(const QByteArray geometry,
                                                 const QByteArray state)
{
    currentGeometry = geometry;
    currentState    = state;
}

const QByteArray& MockMainWindowView::getState() const
{
    return currentState;
}

const QByteArray& MockMainWindowView::getGeometry() const
{
    return currentGeometry;
}
