
#include "aboutaideusecase.hpp"

#include <utility>

#include <QFile>

#include <aide/application.hpp>
#include <aide/githubrepository.hpp>

#include "aideinformation.hpp"
#include "aideinformationbuilder.hpp"

using aide::core::AboutAideUseCase;

AboutAideUseCase::AboutAideUseCase(AideInformationPresenterWeakPtr presenter,
                                   LoggerPtr logger)
    : m_presenter{std::move(presenter)}
    , m_logger{std::move(logger)}
{}

void AboutAideUseCase::showAboutAideInformation() const
{
    AideInformation info = AideInformationBuilder::buildCurrent();

    info.whatsNewUrl = std::string(constants::GITHUB_REPO_URL) +
                       "/releases/tag/v" + info.versionInfo;

    info.thirdPartyLicensesHtml = getThirdPartyLicenses();

    if (!m_presenter.expired()) {
        const auto sharedPtr{m_presenter.lock()};
        sharedPtr->showAboutInformation(info);
    }
}

std::string AboutAideUseCase::getThirdPartyLicenses() const
{
    QFile file(QString(INSTALL_LICENSE_FILE));

    if (!file.open(QIODevice::ReadOnly)) {
        file.setFileName(QString(DEVEL_LICENSE_FILE));
        if (file.open(QIODevice::ReadOnly)) {
            m_logger->debug("Parsing third party licenses file {}",
                            file.fileName().toStdString());
            return file.readAll().toStdString();
        }
        m_logger->warn("Could not find a valid third party licenses file");
        return "<b>No third-party licenses file found</b>";
    }
    m_logger->debug("Parsing third party licenses file {}",
                    file.fileName().toStdString());

    return file.readAll().toStdString();
}
