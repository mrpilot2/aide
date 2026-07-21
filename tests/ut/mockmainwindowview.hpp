#ifndef AIDE_MOCK_MAIN_WINDOW_VIEW_HPP
#define AIDE_MOCK_MAIN_WINDOW_VIEW_HPP

#include <QByteArray>

#include <aide/mainwindowinterface.hpp>

namespace aide::test
{
    class MockMainWindowView : public aide::core::MainWindowInterface
    {
    public:
        void restoreGeometryAndState(QByteArray geometry,
                                     QByteArray state) override;

        aide::widgets::Banner* addBanner(NotificationType type,
                                         const QString& message) override;

        void removeBanner(aide::widgets::Banner* banner) override;

        [[nodiscard]] aide::widgets::NotificationView* notificationLogView()
            const override;

        [[nodiscard]] const QByteArray& getState() const;

        [[nodiscard]] const QByteArray& getGeometry() const;

    private:
        QByteArray currentState;
        QByteArray currentGeometry;
    };
} // namespace aide::test

#endif // AIDE_MOCK_MAIN_WINDOW_VIEW_HPP
