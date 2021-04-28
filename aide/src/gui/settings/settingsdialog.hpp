#ifndef AIDE_SETTINGS_DIALOG_HPP
#define AIDE_SETTINGS_DIALOG_HPP

#include <memory>

#include <QDialog>

#include "settings/settingsdialoggeometryandstatecontroller.hpp"
#include "settings/settingsdialoginterface.hpp"
#include "settingsdialogcontroller.hpp"
#include "userselection.hpp"

namespace Ui
{
    class SettingsDialog;
} // namespace Ui

class QWidget;

namespace aide::gui
{
    class SettingsDialog
        : public QDialog
        , public aide::core::SettingsDialogInterface
    {
    public:
        explicit SettingsDialog(QWidget* parent = nullptr);
        ~SettingsDialog() override;
        SettingsDialog(const SettingsDialog&) = delete;
        SettingsDialog& operator=(const SettingsDialog&) = delete;
        SettingsDialog(SettingsDialog&&)                 = delete;
        SettingsDialog& operator=(SettingsDialog&&) = delete;

        void setController(SettingsDialogControllerPtr controller);

        void setTreeModel(std::shared_ptr<QAbstractItemModel> model) override;

        void restoreGeometryAndState(
            aide::core::SettingsDialogGeometryAndStateData geometryAndStateData)
            override;

        void setSelectedGroupIndex(const QModelIndex& index) override;

        [[nodiscard]] aide::core::SettingsDialogGeometryAndStateData
        currentGeometry() const override;

        aide::core::UserSelection executeDialog() override;

        void setSelectedPageDisplayName(
            const std::string& displayName) override;

        void showSelectedPageWidget(QWidget* widget) override;

        void showEmptyPageWidget() override;

        void showResetLabel(bool show) override;

        void enableApplyButton(bool enable) override;

    private:
        void connectSignals();

        void installChangeDetector(QObject* widget);

        void unInstallChangeDetector(QObject* widget);

        std::unique_ptr<Ui::SettingsDialog> ui;

        SettingsDialogControllerPtr settingsController;
    };

} // namespace aide::gui

#endif // AIDE_SETTINGS_DIALOG_HPP
