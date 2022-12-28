
#ifndef AIDE_AIDE_INFORMATION_PRESENTER_INTERFACE_HPP
#define AIDE_AIDE_INFORMATION_PRESENTER_INTERFACE_HPP

#include <memory>

namespace aide::core
{
    struct AideInformation;

    class AideInformationPresenter
    {
    public:
        virtual ~AideInformationPresenter() = default;

        virtual void showAboutInformation(const AideInformation& info) = 0;
    };

    using AideInformationPresenterPtr =
        std::shared_ptr<AideInformationPresenter>;
    using AideInformationPresenterWeakPtr =
        std::weak_ptr<AideInformationPresenter>;
} // namespace aide::core

#endif // AIDE_AIDE_INFORMATION_PRESENTER_INTERFACE_HPP
