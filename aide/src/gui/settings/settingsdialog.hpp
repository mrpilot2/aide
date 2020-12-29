#ifndef AIDE_SETTINGS_DIALOG_HPP
#define AIDE_SETTINGS_DIALOG_HPP

#include <memory>

#include <QDialog>

#include "settings/settingsdialoginterface.hpp"

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

        void executeDialog() override;

    private:
        std::unique_ptr<Ui::SettingsDialog> m_ui;
    };

} // namespace aide::gui

#endif // AIDE_SETTINGS_DIALOG_HPP
