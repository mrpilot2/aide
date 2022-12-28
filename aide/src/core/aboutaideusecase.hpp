
#ifndef AIDE_ABOUT_AIDE_USE_CASE_HPP
#define AIDE_ABOUT_AIDE_USE_CASE_HPP

#include "aideinformationpresenter.hpp"

namespace aide::core
{
    class AboutAideUseCase
    {
    public:
        explicit AboutAideUseCase(AideInformationPresenterWeakPtr presenter);

        void showAboutAideInformation();

    private:
        AideInformationPresenterWeakPtr m_presenter;
    };
} // namespace aide::core

#endif // AIDE_ABOUT_AIDE_USE_CASE_HPP
