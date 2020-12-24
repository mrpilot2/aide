#include "mockmainwindowview.hpp"

using aide::test::MockMainWindowView;

void MockMainWindowView::restoreGeometryAndState(QByteArray geometry,
                                                 QByteArray state)
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
