#include "logger/log_helper_macros.hpp"

std::shared_ptr<spdlog::logger> LoggerMacros::getLogger(const std::string& name)
{
    return spdlog::get(name);
}

void LoggerMacros::registerLogger(const std::shared_ptr<spdlog::logger>& logger)
{
    spdlog::register_logger(logger);
}

void LoggerMacros::dropLogger(const std::string& name)
{
    spdlog::drop(name);
}

void LoggerMacros::dropAllLoggers()
{
    spdlog::drop_all();
}
