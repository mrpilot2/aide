
#include <catch2/catch.hpp>

#include <aide/buildinformation.hpp>

#include "aboutaideusecase.hpp"
#include "mockaboutdialog.hpp"
#include "nulllogger.hpp"

using aide::core::AboutAideUseCase;
using aide::tests::MockAboutDialog;

TEST_CASE("Any AboutAideUseCase")
{
    std::shared_ptr<MockAboutDialog> const dialog{
        std::make_shared<MockAboutDialog>()};

    AboutAideUseCase useCase{dialog,
                             std::make_shared<aide::test::NullLogger>()};

    useCase.showAboutAideInformation();

    SECTION(" passes correct version number to dialog")
    {
        REQUIRE(dialog->getInfo().versionInfo ==
                aide::build_information::AIDE_VERSION_STRING);
    }

    SECTION(" passes correct what's new URL")
    {
        REQUIRE(dialog->getInfo().whatsNewUrl ==
                "https://github.com/mrpilot2/aide/releases/tag/" +
                    std::string("v") +
                    aide::build_information::AIDE_VERSION_STRING);
    }

    SECTION(" passes correct git hash")
    {
        REQUIRE(dialog->getInfo().gitHash == aide::build_information::GIT_HASH);
    }
}
