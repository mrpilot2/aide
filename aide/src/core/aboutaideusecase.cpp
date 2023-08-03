
#include "aboutaideusecase.hpp"

#include <utility>

#include <QFile>
#include <QLocale>

#include <aide/application.hpp>
#include <aide/buildinformation.hpp>

#include "aideinformation.hpp"

using aide::core::AboutAideUseCase;

AboutAideUseCase::AboutAideUseCase(AideInformationPresenterWeakPtr presenter,
                                   LoggerPtr logger)
    : m_presenter{std::move(presenter)}
    , m_logger{std::move(logger)}
{}

void AboutAideUseCase::showAboutAideInformation() const
{
    AideInformation info;

    info.versionInfo = aide::build_information::AIDE_VERSION_STRING;
    info.gitHash     = aide::build_information::GIT_HASH;

    info.buildDate =
        QLocale("en_US").toDate(QString(__DATE__).simplified(), "MMM d yyyy");
    info.compiler        = aide::build_information::CMAKE_CXX_COMPILER;
    info.compilerVersion = aide::build_information::CMAKE_CXX_COMPILER_VERSION;
    info.buildType       = aide::build_information::CMAKE_BUILD_TYPE;
    info.compileFlags    = aide::build_information::COMPILE_FLAGS;

    info.whatsNewUrl =
        "https://github.com/mrpilot2/aide/releases/tag/v" + info.versionInfo;

    info.thirdPartyLicensesHtml = getThirdPartyLicenses();

    if (!m_presenter.expired()) {
        auto sharedPtr{m_presenter.lock()};
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
