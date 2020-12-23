#include "applicationclose.hpp"

#include <utility>

#include "hierarchicalid.hpp"
#include "settingsinterface.hpp"

using aide::HierarchicalId;
using aide::SettingsInterface;
using aide::core::ApplicationClose;
using aide::core::ApplicationCloseViewWeakPtr;

ApplicationClose::ApplicationClose(ApplicationCloseViewWeakPtr v,
                                   SettingsInterface& settings)
    : view{std::move(v)}
    , settings{settings}
{}

bool ApplicationClose::isCloseAllowed() const
{
    if (view.expired()) { return true; }

    auto ptr = view.lock();

    const auto askExitConfirmationKeyGroup =
        HierarchicalId("System")("Behavior");

    if (settings.value(askExitConfirmationKeyGroup, "AskExitConfirmation", true)
            .toBool()) {
        return ptr->letUserConfirmApplicationClose();
    }

    return true;
}
