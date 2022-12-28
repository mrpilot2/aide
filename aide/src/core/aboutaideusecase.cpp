

#include "aboutaideusecase.hpp"

#include <utility>

#include <QLocale>

#include <aide/buildinformation.hpp>

#include "aideinformation.hpp"

using aide::core::AboutAideUseCase;

AboutAideUseCase::AboutAideUseCase(AideInformationPresenterWeakPtr presenter)
    : m_presenter{std::move(presenter)}
{}

void AboutAideUseCase::showAboutAideInformation()
{
    AideInformation info;

    info.versionInfo = aide::build_information::AIDE_VERSION_STRING;
    info.gitHash     = aide::build_information::GIT_HASH;
    info.buildDate   = QLocale("en_US").toDate(__DATE__, "MMM d yyyy");

    info.whatsNewUrl =
        "https://github.com/mrpilot2/aide/releases/tag/v" + info.versionInfo;

    if (!m_presenter.expired()) {
        auto sharedPtr{m_presenter.lock()};
        sharedPtr->showAboutInformation(info);
    }
}
