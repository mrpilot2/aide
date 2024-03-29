

#ifndef AIDE_THIRD_PARTY_LICENSES_DIALOG_HPP
#define AIDE_THIRD_PARTY_LICENSES_DIALOG_HPP

#include <memory>

#include <QDialog>

namespace Ui
{
    class ThirdPartyLicensesDialog;
} // namespace Ui

namespace aide::gui
{
    class ThirdPartyLicensesDialog : public QDialog
    {
    public:
        explicit ThirdPartyLicensesDialog(QWidget* parent = nullptr);

        ~ThirdPartyLicensesDialog() override;

        void setLicensesText(const QString& text) const;

    private:
        std::unique_ptr<Ui::ThirdPartyLicensesDialog> m_ui;
    };
} // namespace aide::gui

#endif // AIDE_THIRD_PARTY_LICENSES_DIALOG_HPP
