
#ifndef AIDE_LOG_HELPER_MACROS_HPP
#define AIDE_LOG_HELPER_MACROS_HPP

#include <spdlog/spdlog.h>

class LoggerMacros
{
public:
    static std::shared_ptr<spdlog::logger> getLogger(const std::string& name);
};

const char* const AIDE_DEFAULT_MACRO_LOGGER{"aide_macro"};

// NOLINTNEXTLINE
#define INVALID_LOGGER_REQUEST(logger_name)                                    \
    if (LoggerMacros::getLogger(AIDE_DEFAULT_MACRO_LOGGER) != nullptr) {       \
        SPDLOG_LOGGER_ERROR(                                                   \
            LoggerMacros::getLogger(AIDE_DEFAULT_MACRO_LOGGER),                \
            "You are trying to access logger \"{}\" with "                     \
            "AIDE_CUSTOM_LOG_INFO that does not exists. "                      \
            "Cannot use the requested logger, using default "                  \
            "as fallback.\nRelevant code  position: {}:{}",                    \
            logger_name, __FILE__, __LINE__);                                  \
    }

// NOLINTNEXTLINE
#define AIDE_CUSTOM_LOG_TRACE(logger_name, ...)                                \
    if (LoggerMacros::getLogger(logger_name) != nullptr) {                     \
        SPDLOG_LOGGER_TRACE(LoggerMacros::getLogger(logger_name),              \
                            __VA_ARGS__);                                      \
    } else {                                                                   \
        INVALID_LOGGER_REQUEST(logger_name)                                    \
        if (LoggerMacros::getLogger(AIDE_DEFAULT_MACRO_LOGGER) != nullptr) {   \
            SPDLOG_LOGGER_TRACE(                                               \
                LoggerMacros::getLogger(AIDE_DEFAULT_MACRO_LOGGER),            \
                __VA_ARGS__);                                                  \
        }                                                                      \
    }

// NOLINTNEXTLINE
#define AIDE_LOG_TRACE(...)                                                    \
    AIDE_CUSTOM_LOG_TRACE(AIDE_DEFAULT_MACRO_LOGGER, __VA_ARGS__);

// NOLINTNEXTLINE
#define AIDE_CUSTOM_LOG_DEBUG(logger_name, ...)                                \
    if (LoggerMacros::getLogger(logger_name) != nullptr) {                     \
        SPDLOG_LOGGER_DEBUG(LoggerMacros::getLogger(logger_name),              \
                            __VA_ARGS__);                                      \
    } else {                                                                   \
        INVALID_LOGGER_REQUEST(logger_name)                                    \
        if (LoggerMacros::getLogger(AIDE_DEFAULT_MACRO_LOGGER) != nullptr) {   \
            SPDLOG_LOGGER_DEBUG(                                               \
                LoggerMacros::getLogger(AIDE_DEFAULT_MACRO_LOGGER),            \
                __VA_ARGS__);                                                  \
        }                                                                      \
    }

// NOLINTNEXTLINE
#define AIDE_LOG_DEBUG(...)                                                    \
    AIDE_CUSTOM_LOG_DEBUG(AIDE_DEFAULT_MACRO_LOGGER, __VA_ARGS__);

// NOLINTNEXTLINE
#define AIDE_CUSTOM_LOG_INFO(logger_name, ...)                                 \
    if (LoggerMacros::getLogger(logger_name) != nullptr) {                     \
        SPDLOG_LOGGER_INFO(LoggerMacros::getLogger(logger_name), __VA_ARGS__); \
    } else {                                                                   \
        INVALID_LOGGER_REQUEST(logger_name)                                    \
        if (LoggerMacros::getLogger(AIDE_DEFAULT_MACRO_LOGGER) != nullptr) {   \
            SPDLOG_LOGGER_INFO(                                                \
                LoggerMacros::getLogger(AIDE_DEFAULT_MACRO_LOGGER),            \
                __VA_ARGS__);                                                  \
        }                                                                      \
    }
// NOLINTNEXTLINE
#define AIDE_LOG_INFO(...)                                                     \
    AIDE_CUSTOM_LOG_INFO(AIDE_DEFAULT_MACRO_LOGGER, __VA_ARGS__);

// NOLINTNEXTLINE
#define AIDE_CUSTOM_LOG_WARN(logger_name, ...)                                 \
    if (LoggerMacros::getLogger(logger_name) != nullptr) {                     \
        SPDLOG_LOGGER_WARN(LoggerMacros::getLogger(logger_name), __VA_ARGS__); \
    } else {                                                                   \
        INVALID_LOGGER_REQUEST(logger_name)                                    \
        if (LoggerMacros::getLogger(AIDE_DEFAULT_MACRO_LOGGER) != nullptr) {   \
            SPDLOG_LOGGER_WARN(                                                \
                LoggerMacros::getLogger(AIDE_DEFAULT_MACRO_LOGGER),            \
                __VA_ARGS__);                                                  \
        }                                                                      \
    }

// NOLINTNEXTLINE
#define AIDE_LOG_WARN(...)                                                     \
    AIDE_CUSTOM_LOG_WARN(AIDE_DEFAULT_MACRO_LOGGER, __VA_ARGS__);

// NOLINTNEXTLINE
#define AIDE_CUSTOM_LOG_ERROR(logger_name, ...)                                \
    if (LoggerMacros::getLogger(logger_name) != nullptr) {                     \
        SPDLOG_LOGGER_ERROR(LoggerMacros::getLogger(logger_name),              \
                            __VA_ARGS__);                                      \
    } else {                                                                   \
        INVALID_LOGGER_REQUEST(logger_name)                                    \
        if (LoggerMacros::getLogger(AIDE_DEFAULT_MACRO_LOGGER) != nullptr) {   \
            SPDLOG_LOGGER_ERROR(                                               \
                LoggerMacros::getLogger(AIDE_DEFAULT_MACRO_LOGGER),            \
                __VA_ARGS__);                                                  \
        }                                                                      \
    }

// NOLINTNEXTLINE
#define AIDE_LOG_ERROR(...)                                                    \
    AIDE_CUSTOM_LOG_ERROR(AIDE_DEFAULT_MACRO_LOGGER, __VA_ARGS__);

// NOLINTNEXTLINE
#define AIDE_CUSTOM_LOG_CRITICAL(logger_name, ...)                             \
    if (LoggerMacros::getLogger(logger_name) != nullptr) {                     \
        SPDLOG_LOGGER_CRITICAL(LoggerMacros::getLogger(logger_name),           \
                               __VA_ARGS__);                                   \
    } else {                                                                   \
        INVALID_LOGGER_REQUEST(logger_name)                                    \
        if (LoggerMacros::getLogger(AIDE_DEFAULT_MACRO_LOGGER) != nullptr) {   \
            SPDLOG_LOGGER_CRITICAL(                                            \
                LoggerMacros::getLogger(AIDE_DEFAULT_MACRO_LOGGER),            \
                __VA_ARGS__);                                                  \
        }                                                                      \
    }

// NOLINTNEXTLINE
#define AIDE_LOG_CRITICAL(...)                                                 \
    AIDE_CUSTOM_LOG_CRITICAL(AIDE_DEFAULT_MACRO_LOGGER, __VA_ARGS__);

#endif // AIDE_LOG_HELPER_MACROS_HPP
