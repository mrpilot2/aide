#ifndef AIDE_CHANGE_DETECTOR_HPP
#define AIDE_CHANGE_DETECTOR_HPP

#include "settingsdialogcontroller.hpp"

class QObject;

namespace aide::gui
{
    void installChangeDetector(const QObject* widget,
                               const SettingsDialogControllerPtr& controller);

    void unInstallChangeDetector(const QObject* widget,
                                 const SettingsDialogControllerPtr& controller);

} // namespace aide::gui

#endif // AIDE_CHANGE_DETECTOR_HPP
