#include "log_helper_macros.hpp"

std::shared_ptr<spdlog::logger> LoggerMacros::getLogger(const std::string& name)
{
    return spdlog::get(name);
}
