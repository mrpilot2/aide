#include "aidedialog.hpp"

#include <QVBoxLayout>

#include "banner.hpp"

namespace aide::widgets
{
    namespace
    {
        constexpr int NO_MARGIN{0};
    } // namespace

    AideDialog::AideDialog(QWidget* parent)
        : QDialog(parent)
        , m_bannerSlotLayout(new QVBoxLayout())
        , m_contentLayout(new QVBoxLayout())
    {
        auto* rootLayout = new QVBoxLayout(this);
        rootLayout->setContentsMargins(NO_MARGIN, NO_MARGIN, NO_MARGIN,
                                       NO_MARGIN);
        rootLayout->setSpacing(NO_MARGIN);

        m_bannerSlotLayout->setContentsMargins(NO_MARGIN, NO_MARGIN, NO_MARGIN,
                                               NO_MARGIN);
        m_bannerSlotLayout->setSpacing(NO_MARGIN);
        rootLayout->addLayout(m_bannerSlotLayout);

        rootLayout->addLayout(m_contentLayout, 1);
    }

    Banner* AideDialog::showBanner(NotificationType type,
                                   const QString& message)
    {
        clearBanner();

        m_banner = new Banner(type, message, this);
        connect(m_banner, &Banner::closed, this, &AideDialog::clearBanner);
        m_bannerSlotLayout->addWidget(m_banner);

        return m_banner;
    }

    void AideDialog::clearBanner()
    {
        if (m_banner == nullptr) { return; }

        m_bannerSlotLayout->removeWidget(m_banner);
        // Detach immediately rather than waiting for the deferred deletion
        // to run, so the banner is no longer reachable (e.g. via
        // findChildren()) as soon as clearBanner() returns.
        m_banner->setParent(nullptr);
        m_banner->deleteLater();
        m_banner = nullptr;
    }

    QVBoxLayout* AideDialog::contentLayout() const
    {
        return m_contentLayout;
    }
} // namespace aide::widgets
