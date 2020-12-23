#ifndef AIDE_APPLICATION_CLOSE_CONTROLLER_HPP
#define AIDE_APPLICATION_CLOSE_CONTROLLER_HPP

namespace aide::core
{
    class ApplicationCloseController
    {
    public:
        virtual ~ApplicationCloseController() = default;

        [[nodiscard]] virtual bool isCloseAllowed() const = 0;
    };
} // namespace aide::core

#endif // AIDE_APPLICATION_CLOSE_INPUT_HPP
