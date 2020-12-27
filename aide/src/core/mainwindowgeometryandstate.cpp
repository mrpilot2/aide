#include "mainwindowgeometryandstate.hpp"

#include <utility>

#include "hierarchicalid.hpp"
#include "settingsinterface.hpp"

using aide::core::MainWindowGeometryAndState;
using aide::core::MainWindowInterfaceWeakPtr;

MainWindowGeometryAndState::MainWindowGeometryAndState(
    MainWindowInterfaceWeakPtr v,
    aide::SettingsInterface& unversionableSettings)
    : view{std::move(v)}
    , settings{unversionableSettings}
{}

void MainWindowGeometryAndState::saveGeometryAndState(QByteArray geometry,
                                                      QByteArray state)
{
    auto geometryKey = HierarchicalId("Geometry")("MainWindow");
    auto stateKey    = HierarchicalId("State")("MainWindow");

    settings.setValue(geometryKey, geometry);
    settings.setValue(stateKey, state);
}

void MainWindowGeometryAndState::restoreGeometryAndState()
{
    auto geometryKey = HierarchicalId("Geometry")("MainWindow");
    auto stateKey    = HierarchicalId("State")("MainWindow");

    auto geometry = settings.value(geometryKey).toByteArray();
    auto state    = settings.value(stateKey).toByteArray();

    {
        auto ptr = view.lock();
        if (ptr == nullptr) { return; }

        if (geometry.isEmpty() && state.isEmpty()) {
            ptr->showMaximized();
        } else {
            ptr->restoreGeometryAndState(geometry, state);
            ptr->show();
        }
    }
}
