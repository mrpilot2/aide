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
    auto geometryGroup = HierarchicalId("Geometry");
    auto stateGroup    = HierarchicalId("State");

    settings.setValue(geometryGroup, "MainWindow", geometry);
    settings.setValue(stateGroup, "MainWindow", state);
}

void MainWindowGeometryAndState::restoreGeometryAndState()
{
    auto geometryGroup = HierarchicalId("Geometry");
    auto stateGroup    = HierarchicalId("State");

    auto geometry = settings.value(geometryGroup, "MainWindow").toByteArray();
    auto state    = settings.value(stateGroup, "MainWindow").toByteArray();

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
