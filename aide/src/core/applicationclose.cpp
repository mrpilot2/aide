#include "applicationclose.hpp"

#include <utility>

#include <QApplication>

#include "hierarchicalid.hpp"
#include "settingsinterface.hpp"

using aide::HierarchicalId;
using aide::SettingsInterface;
using aide::core::ApplicationClose;
using aide::core::ApplicationCloseViewWeakPtr;

ApplicationClose::ApplicationClose(ApplicationCloseViewWeakPtr v,
                                   SettingsInterface& s)
    : view{std::move(v)}
    , settings{s}
{}

bool ApplicationClose::isCloseAllowed() const
{
    auto ptr = view.lock();

    const auto askExitConfirmationKeyGroup =
        HierarchicalId("System")("Behavior");

    if (settings.value(askExitConfirmationKeyGroup, "AskExitConfirmation", true)
            .toBool()) {
        const auto& [userSelection, dontAskAgain] =
            ptr->letUserConfirmApplicationClose();

        if (dontAskAgain) {
            settings.setValue(askExitConfirmationKeyGroup,
                              "AskExitConfirmation", false);
        }
        return userSelection == UserSelection::Exit;
    }

    return true;
}
