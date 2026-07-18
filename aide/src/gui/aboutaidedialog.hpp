

#ifndef AIDE_ABOUT_AIDE_DIALOG_HPP
#define AIDE_ABOUT_AIDE_DIALOG_HPP

#include <memory>

#include <QDialog>

#include "aideinformation.hpp"
#include "aideinformationpresenter.hpp"

namespace Ui
{
    class AboutAideDialog;
} // namespace Ui

class QWidget;

namespace aide::gui
{
    class AboutAideDialog
        : public QDialog
        , public core::AideInformationPresenter
    {
        Q_OBJECT
    public:
        explicit AboutAideDialog(QWidget* parent = nullptr);

        ~AboutAideDialog() override;

        void showAboutInformation(const core::AideInformation& info) override;

        void copySystemInfoToClipBoard() const;

    private slots:
        static void whatsNewHovered(const QString& text);

        void onThirdPartyLibrariesLinkClicked() const;

    private:
        std::unique_ptr<Ui::AboutAideDialog> m_ui;

        core::AideInformation m_info;
    };
} // namespace aide::gui

#endif // AIDE_ABOUT_AIDE_DIALOG_HPP
