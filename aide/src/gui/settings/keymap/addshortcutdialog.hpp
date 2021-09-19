
#ifndef AIDE_ADD_SHORTCUT_DIALOG_HPP
#define AIDE_ADD_SHORTCUT_DIALOG_HPP

#include <memory>

#include <QDialog>

namespace Ui
{
    class AddShortcutDialog;
} // namespace Ui

namespace aide::gui
{
    class AddShortcutDialog : public QDialog
    {
    public:
        explicit AddShortcutDialog(QWidget* parent = nullptr);
        ~AddShortcutDialog() override;

        void setAssignedActionDescription(const QString& text);

        QKeySequence userSelectedKeySequence() const;

    private:
        std::unique_ptr<Ui::AddShortcutDialog> ui;
    };
} // namespace aide::gui

#endif // AIDE_ADD_SHORTCUT_DIALOG_HPP
