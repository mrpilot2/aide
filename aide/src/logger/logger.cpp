#include "logger.hpp"

#include <memory>
#include <utility>
#include <vector>

#include <spdlog/logger.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

using aide::FileName;
using aide::Logger;
using aide::LoggerName;

static constexpr unsigned int maxFileSizeInMB{1024 * 1024 * 5};
static constexpr uint16_t maxNumberOfFiles{3};

Logger::Logger()
    : Logger(FileName("aide.log"), LoggerName("aide"))
{}

Logger::Logger(const FileName& fileName)
    : Logger(fileName, LoggerName("aide"))
{}

Logger::Logger(const LoggerName& loggerName)
    : Logger(FileName("aide.log"), loggerName)
{}

Logger::Logger(const FileName& logFileName, const LoggerName& loggerName)
{
    auto logSinks = createSinks(logFileName());

    const std::string commonLogPattern =
        "%b %d %H:%M:%S.%e [%8t] - %^%8l%$ - %10n - %v";
    m_logger = std::make_shared<spdlog::logger>(loggerName(), begin(logSinks),
                                                end(logSinks));
    m_logger->set_level(spdlog::level::trace);
    m_logger->set_pattern(commonLogPattern);

    auto macroLogSinks = createSinks(logFileName());
    m_macroLogger      = std::make_shared<spdlog::logger>(
        loggerName() + std::string("_macro"), begin(macroLogSinks),
        end(macroLogSinks));
    m_macroLogger->set_level(spdlog::level::trace);
#ifdef NDEBUG
    m_macroLogger->set_pattern(commonLogPattern);
#else
    m_macroLogger->set_pattern(commonLogPattern + " - %@");
#endif
    registerLogger(m_logger);
    registerLogger(m_macroLogger);

    m_logger->info("Configured logger to log to file {}", logFileName());
}

void Logger::flush()
{
    m_logger->flush();
    m_macroLogger->flush();
}

void Logger::registerLogger(const std::shared_ptr<spdlog::logger>& logger)
{
    if (spdlog::get(logger->name()) != nullptr) { return; }
    try {
        spdlog::register_logger(logger);
    }
    catch (const spdlog::spdlog_ex& ex) {
        logger->info("Logger::Logger() - register logger failed {}: ",
                     ex.what());
    }
}
std::vector<spdlog::sink_ptr> Logger::createSinks(std::string logFileName)
{
    std::vector<spdlog::sink_ptr> sinks;
#ifndef NDEBUG
    sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
#endif

    sinks.push_back(std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
        std::move(logFileName), maxFileSizeInMB, maxNumberOfFiles));

    return sinks;
}

void Logger::doLogTrace(std::string_view message) const
{
    m_logger->trace(message);
}

void Logger::doLogDebug(std::string_view message) const
{
    m_logger->debug(message);
}

void aide::Logger::doLogInfo(std::string_view message) const
{
    m_logger->info(message);
}

void aide::Logger::doLogWarn(std::string_view message) const
{
    m_logger->warn(message);
}

void aide::Logger::doLogError(std::string_view message) const
{
    m_logger->error(message);
}

void aide::Logger::doLogCritical(std::string_view message) const
{
    m_logger->critical(message);
}
