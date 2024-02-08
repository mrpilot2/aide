#include "applicationclose.hpp"

#include <utility>

#include "commonsettingskeys.hpp"
#include "settingsinterface.hpp"

using aide::core::ApplicationClose;
using aide::core::ApplicationCloseViewWeakPtr;
using aide::core::settings::KEYS;

ApplicationClose::ApplicationClose(ApplicationCloseViewWeakPtr v,
                                   SettingsInterface& s)
    : view{std::move(v)}
    , settings{s}
{}

bool ApplicationClose::isCloseAllowed() const
{
    const auto ptr = view.lock();

    const auto askForExitConfirmation =
        settings.value(KEYS().SYSTEM.ASK_EXIT_CONFIRMATION, true);

    if (askForExitConfirmation.toBool()) {
        const auto& [userSelection, dontAskAgain] =
            ptr->letUserConfirmApplicationClose();

        if (dontAskAgain) {
            settings.setValue(KEYS().SYSTEM.ASK_EXIT_CONFIRMATION, false);
        }

        return userSelection == UserSelection::Exit;
    }

    return true;
}
