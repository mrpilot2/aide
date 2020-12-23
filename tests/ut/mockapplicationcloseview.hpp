#ifndef AIDE_MOCK_APPLICATION_CLOSE_VIEW_HPP
#define AIDE_MOCK_APPLICATION_CLOSE_VIEW_HPP

#include "applicationcloseview.hpp"

namespace aide::test
{
    class MockApplicationCloseView : public aide::core::ApplicationCloseView
    {
    public:
        bool letUserConfirmApplicationClose() override;

        [[nodiscard]] bool wasUserAsked() const;

    private:
        bool hasAsked{false};
    };
} // namespace aide::test

#endif // AIDE_MOCK_APPLICATION_CLOSE_VIEW_HPP
