#ifndef AIDE_APPLICATION_CLOSE_VIEW_HPP
#define AIDE_APPLICATION_CLOSE_VIEW_HPP

#include <memory>

namespace aide::core
{
    class ApplicationCloseView
    {
    public:
        virtual ~ApplicationCloseView() = default;

        virtual bool letUserConfirmApplicationClose() = 0;
    };

    using ApplicationCloseViewPtr     = std::shared_ptr<ApplicationCloseView>;
    using ApplicationCloseViewWeakPtr = std::weak_ptr<ApplicationCloseView>;

} // namespace aide::core

#endif // AIDE_APPLICATION_CLOSE_VIEW_HPP
