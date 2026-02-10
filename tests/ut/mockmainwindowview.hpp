#ifndef AIDE_MOCK_MAIN_WINDOW_VIEW_HPP
#define AIDE_MOCK_MAIN_WINDOW_VIEW_HPP

#include <QByteArray>

#include "mainwindowinterface.hpp"

namespace aide::test
{
    class MockMainWindowView : public aide::core::MainWindowInterface
    {
    public:
        void restoreGeometryAndState(QByteArray geometry,
                                     QByteArray state) override;

        [[nodiscard]] const QByteArray& getState() const;

        [[nodiscard]] const QByteArray& getGeometry() const;

    private:
        QByteArray currentState;
        QByteArray currentGeometry;
    };
} // namespace aide::test

#endif // AIDE_MOCK_MAIN_WINDOW_VIEW_HPP
