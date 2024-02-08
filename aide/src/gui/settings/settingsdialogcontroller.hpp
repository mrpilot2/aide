#ifndef AIDE_SETTINGS_DIALOG_CONTROLLER_HPP
#define AIDE_SETTINGS_DIALOG_CONTROLLER_HPP

#include <memory>

#include <QItemSelection>
#include <QObject>

#include "settings/settingsdialogchangepagecontroller.hpp"

class QModelIndex;

namespace aide::gui
{
    class SettingsDialogController : public QObject
    {
        Q_OBJECT
    public:
        explicit SettingsDialogController(
            aide::core::SettingsDialogChangePageController&
                changePageController);

    public slots:
        void onUserChangedSelectedPage(const QItemSelection& selected,
                                       const QItemSelection& deselected) const;

        void onUserChangedAGuiElement() const;

        void onUserWantsToResetCurrentPage() const;

        void onUserWantsToApplySettingsPages() const;

    private:
        aide::core::SettingsDialogChangePageController& controller;
    };

    using SettingsDialogControllerPtr =
        std::shared_ptr<SettingsDialogController>;
} // namespace aide::gui

#endif // AIDE_SETTINGS_DIALOG_CONTROLLER_HPP
