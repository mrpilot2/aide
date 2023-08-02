
#ifndef AIDE_ABOUT_AIDE_USE_CASE_HPP
#define AIDE_ABOUT_AIDE_USE_CASE_HPP

#include <string>

#include <aide/loggerinterface.hpp>

#include "aideinformationpresenter.hpp"

namespace aide::core
{
    class AboutAideUseCase
    {
    public:
        explicit AboutAideUseCase(AideInformationPresenterWeakPtr presenter,
                                  LoggerPtr logger);

        void showAboutAideInformation() const;

    private:
        [[nodiscard]] std::string getThirdPartyLicenses() const;

        AideInformationPresenterWeakPtr m_presenter;

        LoggerPtr m_logger;
    };
} // namespace aide::core

#endif // AIDE_ABOUT_AIDE_USE_CASE_HPP
