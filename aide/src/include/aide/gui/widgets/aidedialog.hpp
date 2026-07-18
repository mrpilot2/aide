
#ifndef AIDE_AIDEDIALOG_HPP
#define AIDE_AIDEDIALOG_HPP

#include <QDialog>
#include <QString>

#include <aide/notificationtype.hpp>

class QVBoxLayout;
class QWidget;

namespace aide::widgets
{
    class Banner;

    /**
     * @brief Styled QDialog base exposing a banner slot between the title
     * bar and the content, above the button box. Subclasses add their own
     * content and button box to contentLayout(); showBanner()/clearBanner()
     * manage the shared Banner widget in the slot above it (see #146).
     * New dialogs only - existing dialogs are not retrofitted onto this
     * base.
     */
    class AideDialog : public QDialog
    {
        Q_OBJECT
    public:
        explicit AideDialog(QWidget* parent = nullptr);

        Banner* showBanner(NotificationType type, const QString& message);

        void clearBanner();

    protected:
        [[nodiscard]] QVBoxLayout* contentLayout() const;

    private:
        QVBoxLayout* m_bannerSlotLayout;
        QVBoxLayout* m_contentLayout;
        Banner* m_banner{nullptr};
    };
} // namespace aide::widgets

#endif // AIDE_AIDEDIALOG_HPP
