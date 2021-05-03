#ifndef AIDE_CHANGE_DETECTOR_HPP
#define AIDE_CHANGE_DETECTOR_HPP

#include <QObject>

#include "settingsdialogcontroller.hpp"

namespace aide::gui
{
    void installChangeDetector(QObject* widget,
                               const SettingsDialogControllerPtr& controller);

    void unInstallChangeDetector(QObject* widget,
                                 const SettingsDialogControllerPtr& controller);

} // namespace aide::gui

#endif // AIDE_CHANGE_DETECTOR_HPP
