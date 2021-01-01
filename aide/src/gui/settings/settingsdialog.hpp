#ifndef AIDE_SETTINGS_DIALOG_HPP
#define AIDE_SETTINGS_DIALOG_HPP

#include <memory>

#include <QDialog>

#include "settings/settingsdialoginterface.hpp"
#include "settingsdialogcontroller.hpp"

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
        ~SettingsDialog();
        SettingsDialog(const SettingsDialog&) = delete;
        SettingsDialog& operator=(const SettingsDialog&) = delete;
        SettingsDialog(SettingsDialog&&)                 = delete;
        SettingsDialog& operator=(SettingsDialog&&) = delete;

        void setController(SettingsDialogControllerPtr controller);

        void setTreeModel(std::shared_ptr<QAbstractItemModel> model) override;

        void executeDialog() override;

        void setSelectedPageDisplayName(
            const std::string& displayName) override;

        void showSelectedPageWidget(QWidget* widget) override;

        void showEmptyPageWidget() override;

        void showResetLabel(bool show) override;

        void enableApplyButton(bool enable) override;

    private:
        void connectSignals();

        std::unique_ptr<Ui::SettingsDialog> ui;

        SettingsDialogControllerPtr settingsController;
        void installChangeDetector(QObject* widget);
    };

} // namespace aide::gui

#endif // AIDE_SETTINGS_DIALOG_HPP
