#include "logger.hpp"

#include <memory>
#include <vector>

#include <spdlog/logger.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

using aide::Logger;

Logger::Logger()
{
    std::vector<spdlog::sink_ptr> sinks;
    sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());

    const int maxFileSize = 1048576 * 5;

    sinks.push_back(std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
        "logfile.txt", maxFileSize, 3));

    m_logger = std::make_shared<spdlog::logger>("aide_logger", begin(sinks),
                                                end(sinks));
}
