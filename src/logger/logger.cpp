#include "logger.hpp"

#include <memory>
#include <utility>
#include <vector>

#include <spdlog/logger.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

using aide::Logger;

static constexpr unsigned int maxFileSizeInMB{1024 * 1024 * 5};
static constexpr uint16_t maxNumberOfFiles{3};

Logger::Logger()
    : Logger("aide.log")
{
}

Logger::Logger(const std::string& logFileName)
{
    auto logSinks = createSinks(logFileName);
    m_logger = std::make_shared<spdlog::logger>("aide_logger", begin(logSinks),
                                                end(logSinks));
    m_logger->set_level(spdlog::level::trace);
    m_logger->set_pattern("%Y-%m-%d %H:%M:%S%e [%8t] - %8l - %n - %v");

    auto macroLogSinks = createSinks(logFileName);
    m_macroLogger      = std::make_shared<spdlog::logger>(
        AIDE_DEFAULT_MACRO_LOGGER, begin(macroLogSinks), end(macroLogSinks));
    m_macroLogger->set_level(spdlog::level::trace);
    m_macroLogger->set_pattern(
        "%Y-%m-%d %H:%M:%S%e [%8t] - %8l - %n - %g:%# - %v");

    registerLogger(m_logger);
    registerLogger(m_macroLogger);
}

void Logger::flush()
{
    m_logger->flush();
}

void Logger::registerLogger(std::shared_ptr<spdlog::logger> logger)
{
    try {
        spdlog::register_logger(std::move(logger));
    }
    catch (const spdlog::spdlog_ex& ex) {
        m_logger->info("Logger::Logger() - register macro logger failed {}: ",
                       ex.what());
    }
}

std::vector<spdlog::sink_ptr> Logger::createSinks(std::string logFileName)
{
    std::vector<spdlog::sink_ptr> sinks;
    sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());

    sinks.push_back(std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
        std::move(logFileName), maxFileSizeInMB, maxNumberOfFiles));

    return sinks;
}
