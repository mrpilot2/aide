#include "mockapplicationcloseview.hpp"

using aide::core::UserSelection;
using aide::test::MockApplicationCloseView;

std::tuple<aide::core::UserSelection, bool>
MockApplicationCloseView::letUserConfirmApplicationClose()
{
    hasAsked = true;

    return std::make_tuple(
        simulateExitReply ? UserSelection::Exit : UserSelection::Cancel,
        doNotAskAgain);
}

bool MockApplicationCloseView::wasUserAsked() const
{
    return hasAsked;
}

void MockApplicationCloseView::userShallClickExit()
{
    simulateExitReply = true;
}

void MockApplicationCloseView::userShallClickCancel()
{
    simulateExitReply = false;
}

void MockApplicationCloseView::userShallSelectToNotBeAskedAgain()
{
    doNotAskAgain = true;
}

void MockApplicationCloseView::userShallSelectToBeAskedAgain()
{
    doNotAskAgain = false;
}
