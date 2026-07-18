
#include "aideinformationbuilder.hpp"

#include <QLocale>
#include <QString>

#include <aide/buildinformation.hpp>

#include "aideinformation.hpp"

using aide::core::AideInformation;
using aide::core::AideInformationBuilder;

AideInformation AideInformationBuilder::buildCurrent()
{
    namespace build_information = aide::build_information;

    AideInformation info;
    info.versionInfo = build_information::AIDE_VERSION_STRING;
    info.gitHash     = build_information::GIT_HASH;
    info.buildDate =
        QLocale("en_US").toDate(QString(__DATE__).simplified(), "MMM d yyyy");
    info.compiler        = build_information::CMAKE_CXX_COMPILER;
    info.compilerVersion = build_information::CMAKE_CXX_COMPILER_VERSION;
    info.buildType       = build_information::CMAKE_BUILD_TYPE;
    info.compileFlags    = build_information::COMPILE_FLAGS;
    return info;
}
