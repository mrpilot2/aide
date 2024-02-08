#include "mainwindowgeometryandstate.hpp"

#include <utility>

#include "commonsettingskeys.hpp"
#include "settingsinterface.hpp"

using aide::core::MainWindowGeometryAndState;
using aide::core::MainWindowInterfaceWeakPtr;
using aide::core::settings::KEYS;

MainWindowGeometryAndState::MainWindowGeometryAndState(
    MainWindowInterfaceWeakPtr v, SettingsInterface& unversionableSettings)
    : view{std::move(v)}
    , settings{unversionableSettings}
{}

void MainWindowGeometryAndState::saveGeometryAndState(
    const QByteArray& geometry, const QByteArray& state)
{
    settings.setValue(KEYS().UI.MAIN_WINDOW_GEOMETRY_KEY, geometry);
    settings.setValue(KEYS().UI.MAIN_WINDOW_STATE_KEY, state);
}

void MainWindowGeometryAndState::restoreGeometryAndState()
{
    const auto geometry =
        settings.value(KEYS().UI.MAIN_WINDOW_GEOMETRY_KEY).toByteArray();
    const auto state =
        settings.value(KEYS().UI.MAIN_WINDOW_STATE_KEY).toByteArray();

    {
        const auto ptr = view.lock();
        if (ptr == nullptr) { return; }

        if (geometry.isEmpty() && state.isEmpty()) {
            ptr->showMaximized();
        } else {
            ptr->restoreGeometryAndState(geometry, state);
            ptr->show();
        }
    }
}
