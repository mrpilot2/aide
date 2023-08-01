#ifndef AIDE_SETTINGS_DIALOG_GEOMETRY_AND_STATE_CONTROLLER_HPP
#define AIDE_SETTINGS_DIALOG_GEOMETRY_AND_STATE_CONTROLLER_HPP

#include <QByteArray>

#include "geometryandstatedata.hpp"

namespace aide::core
{
    class SettingsDialogGeometryAndStateController
    {
    public:
        virtual ~SettingsDialogGeometryAndStateController() = default;

        virtual void saveGeometryAndState(
            const SettingsDialogGeometryAndStateData& geometryAndState) = 0;

        virtual void restoreGeometryAndState() = 0;
    };

} // namespace aide::core

#endif // AIDE_SETTINGS_DIALOG_GEOMETRY_AND_STATE_CONTROLLER_HPP
