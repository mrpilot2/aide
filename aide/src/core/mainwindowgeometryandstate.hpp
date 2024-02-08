#ifndef AIDE_MAIN_WINDOW_GEOMETRY_AND_STATE_HPP
#define AIDE_MAIN_WINDOW_GEOMETRY_AND_STATE_HPP

#include "mainwindowgeometryandstatecontroller.hpp"
#include "mainwindowinterface.hpp"

namespace aide
{
    class SettingsInterface;
} // namespace aide

namespace aide::core
{
    class MainWindowGeometryAndState
        : public MainWindowGeometryAndStateController
    {
    public:
        MainWindowGeometryAndState(MainWindowInterfaceWeakPtr v,
                                   SettingsInterface& unversionableSettings);

        void saveGeometryAndState(const QByteArray& geometry,
                                  const QByteArray& state) override;

        void restoreGeometryAndState() override;

    private:
        MainWindowInterfaceWeakPtr view;
        SettingsInterface& settings;
    };
} // namespace aide::core

#endif // AIDE_MAIN_WINDOW_GEOMETRY_AND_STATE_HPP
