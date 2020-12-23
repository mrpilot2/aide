#ifndef AIDE_APPLICATION_CLOSE_HPP
#define AIDE_APPLICATION_CLOSE_HPP

#include "applicationclosecontroller.hpp"
#include "applicationcloseview.hpp"

namespace aide::core
{
    class ApplicationClose : public ApplicationCloseController
    {
    public:
        ApplicationClose(ApplicationCloseViewWeakPtr v);

        [[nodiscard]] bool isCloseAllowed() const override;

    private:
        ApplicationCloseViewWeakPtr view;
    };
} // namespace aide::core

#endif // AIDE_APPLICATION_CLOSE_HPP
