#include "logger.hpp"

#include <memory>
#include <vector>

#include <spdlog/logger.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

using aide::Logger;

static constexpr unsigned int maxFileSizeInMB{1024 * 1024 * 5};
static constexpr uint16_t maxNumberOfFiles{3};

Logger::Logger()
{
    std::vector<spdlog::sink_ptr> sinks;
    sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());

    sinks.push_back(std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
        "logfile.txt", maxFileSizeInMB, maxNumberOfFiles));

    m_logger = std::make_shared<spdlog::logger>("aide_logger", begin(sinks),
                                                end(sinks));
    m_logger->set_level(spdlog::level::trace);
}

void Logger::flush()
{
    m_logger->flush();
}
