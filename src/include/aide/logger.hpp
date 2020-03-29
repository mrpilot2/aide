#ifndef AIDE_LOGGER_HPP
#define AIDE_LOGGER_HPP

#include <memory>

namespace spdlog
{
    class logger;
} // namespace spdlog

namespace aide
{
    /**
     * @brief Logging framework
     *
     * This class wraps an external logging framework, spdlog at the moment.
     * Depending on the build type (Release, Debug), this class creates
     * multiple log sinks.
     * For Debug the log sinks are console and a log file
     * For Release it's only the log file
     */
    class Logger
    {
    public:
        Logger();

    private:
        std::shared_ptr<spdlog::logger> m_logger;
    };
} // namespace aide

#endif // AIDE_LOGGER_HPP
