#include "mockmainwindowview.hpp"

using aide::test::MockMainWindowView;

void MockMainWindowView::restoreGeometryAndState(const QByteArray geometry,
                                                 const QByteArray state)
{
    currentGeometry = geometry;
    currentState    = state;
}

QByteArray MockMainWindowView::state() const
{
    return currentState;
}

QByteArray MockMainWindowView::geometry() const
{
    return currentGeometry;
}
