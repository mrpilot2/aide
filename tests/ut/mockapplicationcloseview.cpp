#include "mockapplicationcloseview.hpp"

using aide::test::MockApplicationCloseView;

bool MockApplicationCloseView::letUserConfirmApplicationClose()
{
    hasAsked = true;
    return false;
}

bool MockApplicationCloseView::wasUserAsked() const
{
    return hasAsked;
}
