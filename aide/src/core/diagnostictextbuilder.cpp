
#include "diagnostictextbuilder.hpp"

#include <QLocale>
#include <QSysInfo>
#include <QThread>

#include "aide/utils/systemmemory.hpp"
#include "aideinformation.hpp"

using aide::core::DiagnosticTextBuilder;
using aide::utils::SystemMemory;

namespace
{
    constexpr int BYTE_TO_MEGABYTE = 1024 * 1024;
} // namespace

QString DiagnosticTextBuilder::build(const AideInformation& info)
{
    const auto memoryInBytes{SystemMemory::getAvailableRAMInBytes()};
    const auto memoryInfo{QString("%1 MB").arg(
        memoryInBytes.has_value()
            ? QString::number(memoryInBytes.value() / BYTE_TO_MEGABYTE)
            : "Undefined")};

    const auto locale{QLocale::system()};
    auto diagnosticText{
        QString("aIDE %1\n%2 built on %3\n\nCompiler: %4 %5\nBuild Type: "
                "%6\nCompile Flags: %7\n\nQt Version: %8\nOS: %9\nKernel: "
                "%10\nMemory: %11\nCores: %12\n")
            .arg(QString::fromStdString(info.versionInfo),
                 QString::fromStdString(info.gitHash),
                 locale.toString(info.buildDate,
                                 QLocale::FormatType::LongFormat),
                 QString::fromStdString(info.compiler),
                 QString::fromStdString(info.compilerVersion),
                 QString::fromStdString(info.buildType),
                 QString::fromStdString(info.compileFlags), qVersion(),
                 QSysInfo::prettyProductName(), QSysInfo::kernelVersion(),
                 memoryInfo, QString::number(QThread::idealThreadCount()))};

#ifdef Q_OS_LINUX
    diagnosticText += QString("Desktop environment: %1")
                          .arg(QString(qgetenv("XDG_CURRENT_DESKTOP")));
#endif

    return diagnosticText;
}
