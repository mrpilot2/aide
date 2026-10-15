#include "capturedmacrolog.hpp"

#include <aide/logger/log_helper_macros.hpp>
#include <spdlog/sinks/ostream_sink.h>
#include <spdlog/spdlog.h>

using aide::test::CapturedMacroLog;

CapturedMacroLog::CapturedMacroLog()
    : m_previous(spdlog::get(AIDE_DEFAULT_MACRO_LOGGER))
{
    spdlog::drop(AIDE_DEFAULT_MACRO_LOGGER);
    auto sink   = std::make_shared<spdlog::sinks::ostream_sink_mt>(m_out);
    auto logger = std::make_shared<spdlog::logger>(AIDE_DEFAULT_MACRO_LOGGER,
                                                   std::move(sink));
    logger->set_level(spdlog::level::trace);
    spdlog::register_logger(logger);
}

CapturedMacroLog::~CapturedMacroLog()
{
    spdlog::drop(AIDE_DEFAULT_MACRO_LOGGER);
    if (m_previous != nullptr) { spdlog::register_logger(m_previous); }
}

std::string CapturedMacroLog::text() const
{
    return m_out.str();
}
