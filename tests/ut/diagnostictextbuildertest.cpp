
#include <catch2/catch_test_macros.hpp>

#include <QThread>

#include "aideinformation.hpp"
#include "diagnostictextbuilder.hpp"

using aide::core::AideInformation;
using aide::core::DiagnosticTextBuilder;

namespace
{
    constexpr int kBuildYear{2026};
    constexpr int kBuildMonth{7};
    constexpr int kBuildDay{18};

    AideInformation makeInfo()
    {
        AideInformation info;
        info.versionInfo     = "1.2.3";
        info.gitHash         = "deadbeef";
        info.buildDate       = QDate(kBuildYear, kBuildMonth, kBuildDay);
        info.compiler        = "Clang";
        info.compilerVersion = "18.0";
        info.buildType       = "Debug";
        info.compileFlags    = "-Wall";
        return info;
    }
} // namespace

TEST_CASE("DiagnosticTextBuilder")
{
    const auto info{makeInfo()};

    SECTION("includes version, git hash and build date")
    {
        const auto text{DiagnosticTextBuilder::build(info)};

        REQUIRE(text.contains("aIDE 1.2.3"));
        REQUIRE(text.contains("deadbeef"));
    }

    SECTION("includes compiler, build type and compile flags")
    {
        const auto text{DiagnosticTextBuilder::build(info)};

        REQUIRE(text.contains("Clang 18.0"));
        REQUIRE(text.contains("Debug"));
        REQUIRE(text.contains("-Wall"));
    }

    SECTION("includes Qt version, OS, kernel, memory and core count")
    {
        const auto text{DiagnosticTextBuilder::build(info)};

        REQUIRE(text.contains(qVersion()));
        REQUIRE(text.contains(QSysInfo::prettyProductName()));
        REQUIRE(text.contains(QSysInfo::kernelVersion()));
        REQUIRE(text.contains(QString::number(QThread::idealThreadCount())));
    }
}
