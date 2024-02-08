
#include <filesystem>

#include <catch2/catch.hpp>

#include <aide/buildinformation.hpp>

#include "aboutaideusecase.hpp"
#include "mockaboutdialog.hpp"
#include "nulllogger.hpp"

using aide::core::AboutAideUseCase;
using aide::tests::MockAboutDialog;

TEST_CASE("Any AboutAideUseCase")
{
    auto const dialog{std::make_shared<MockAboutDialog>()};

    AboutAideUseCase useCase{dialog,
                             std::make_shared<aide::test::NullLogger>()};

    SECTION(" passes correct version number to dialog")
    {
        useCase.showAboutAideInformation();

        REQUIRE(dialog->getInfo().versionInfo ==
                aide::build_information::AIDE_VERSION_STRING);
    }

    SECTION(" passes correct what's new URL")
    {
        useCase.showAboutAideInformation();

        REQUIRE(dialog->getInfo().whatsNewUrl ==
                "https://github.com/mrpilot2/aide/releases/tag/" +
                    std::string("v") +
                    aide::build_information::AIDE_VERSION_STRING);
    }

    SECTION(" passes correct git hash")
    {
        useCase.showAboutAideInformation();

        REQUIRE(dialog->getInfo().gitHash == aide::build_information::GIT_HASH);
    }

    SECTION(" passes third party licenses info from development file")
    {
        useCase.showAboutAideInformation();

        REQUIRE(dialog->getInfo().thirdPartyLicensesHtml.find("<table>"));
    }

    SECTION(" passes third party licenses info from development file")
    {
        useCase.showAboutAideInformation();

        REQUIRE(dialog->getInfo().thirdPartyLicensesHtml.find("<table>") !=
                std::string::npos);
    }

    SECTION(" passes message if no license file is found")
    {
        std::filesystem::rename(DEVEL_LICENSE_FILE,
                                std::string(DEVEL_LICENSE_FILE) + ".bak");

        useCase.showAboutAideInformation();

        std::filesystem::rename(std::string(DEVEL_LICENSE_FILE) + ".bak",
                                DEVEL_LICENSE_FILE);

        REQUIRE(dialog->getInfo().thirdPartyLicensesHtml.find(
                    "No third-party licenses file found") != std::string::npos);
    }
}
