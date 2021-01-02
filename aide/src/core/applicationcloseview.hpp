#ifndef AIDE_APPLICATION_CLOSE_VIEW_HPP
#define AIDE_APPLICATION_CLOSE_VIEW_HPP

#include <memory>
#include <tuple>

#include "userselection.hpp"

namespace aide::core
{
    class ApplicationCloseView
    {
    public:
        virtual ~ApplicationCloseView() = default;

        virtual std::tuple<aide::core::UserSelection, bool>
        letUserConfirmApplicationClose() = 0;
    };

    using ApplicationCloseViewPtr     = std::shared_ptr<ApplicationCloseView>;
    using ApplicationCloseViewWeakPtr = std::weak_ptr<ApplicationCloseView>;

} // namespace aide::core

#endif // AIDE_APPLICATION_CLOSE_VIEW_HPP
