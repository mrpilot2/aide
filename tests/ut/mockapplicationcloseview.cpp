#include "mockapplicationcloseview.hpp"

using aide::test::MockApplicationCloseView;

std::tuple<aide::core::UserSelection, bool>
MockApplicationCloseView::letUserConfirmApplicationClose()
{
    hasAsked = true;
    return std::make_tuple(aide::core::UserSelection::Cancel, false);
}

bool MockApplicationCloseView::wasUserAsked() const
{
    return hasAsked;
}
