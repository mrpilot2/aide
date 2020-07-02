//
// Created by markus on 07.04.20.
//

#ifndef AIDE_LOG_HELPER_MACROS_HPP
#define AIDE_LOG_HELPER_MACROS_HPP

const char* const AIDE_DEFAULT_MACRO_LOGGER{"aide_macro"};

// NOLINTNEXTLINE
#define INVALID_LOGGER_REQUEST(logger_name)                                    \
    if (spdlog::get(AIDE_DEFAULT_MACRO_LOGGER) != nullptr) {                   \
        SPDLOG_LOGGER_ERROR(spdlog::get(AIDE_DEFAULT_MACRO_LOGGER),            \
                            "You are trying to access logger \"{}\" with "     \
                            "AIDE_CUSTOM_LOG_INFO that does not exists. "      \
                            "Cannot use the requested logger, using default "  \
                            "as fallback.\nRelevant code  position: {}:{}",    \
                            logger_name, __FILE__, __LINE__);                  \
    }

// NOLINTNEXTLINE
#define AIDE_CUSTOM_LOG_TRACE(logger_name, ...)                                \
    if (spdlog::get(logger_name) != nullptr) {                                 \
        SPDLOG_LOGGER_TRACE(spdlog::get(logger_name), __VA_ARGS__);            \
    } else {                                                                   \
        INVALID_LOGGER_REQUEST(logger_name)                                    \
        if (spdlog::get(AIDE_DEFAULT_MACRO_LOGGER) != nullptr) {               \
            SPDLOG_LOGGER_TRACE(spdlog::get(AIDE_DEFAULT_MACRO_LOGGER),        \
                                __VA_ARGS__);                                  \
        }                                                                      \
    }

// NOLINTNEXTLINE
#define AIDE_LOG_TRACE(...)                                                    \
    AIDE_CUSTOM_LOG_TRACE(AIDE_DEFAULT_MACRO_LOGGER, __VA_ARGS__);

// NOLINTNEXTLINE
#define AIDE_CUSTOM_LOG_DEBUG(logger_name, ...)                                \
    if (spdlog::get(logger_name) != nullptr) {                                 \
        SPDLOG_LOGGER_DEBUG(spdlog::get(logger_name), __VA_ARGS__);            \
    } else {                                                                   \
        INVALID_LOGGER_REQUEST(logger_name)                                    \
        if (spdlog::get(AIDE_DEFAULT_MACRO_LOGGER) != nullptr) {               \
            SPDLOG_LOGGER_DEBUG(spdlog::get(AIDE_DEFAULT_MACRO_LOGGER),        \
                                __VA_ARGS__);                                  \
        }                                                                      \
    }

// NOLINTNEXTLINE
#define AIDE_LOG_DEBUG(...)                                                    \
    AIDE_CUSTOM_LOG_DEBUG(AIDE_DEFAULT_MACRO_LOGGER, __VA_ARGS__);

// NOLINTNEXTLINE
#define AIDE_CUSTOM_LOG_INFO(logger_name, ...)                                 \
    if (spdlog::get(logger_name) != nullptr) {                                 \
        SPDLOG_LOGGER_INFO(spdlog::get(logger_name), __VA_ARGS__);             \
    } else {                                                                   \
        INVALID_LOGGER_REQUEST(logger_name)                                    \
        if (spdlog::get(AIDE_DEFAULT_MACRO_LOGGER) != nullptr) {               \
            SPDLOG_LOGGER_INFO(spdlog::get(AIDE_DEFAULT_MACRO_LOGGER),         \
                               __VA_ARGS__);                                   \
        }                                                                      \
    }
// NOLINTNEXTLINE
#define AIDE_LOG_INFO(...)                                                     \
    AIDE_CUSTOM_LOG_INFO(AIDE_DEFAULT_MACRO_LOGGER, __VA_ARGS__);

// NOLINTNEXTLINE
#define AIDE_CUSTOM_LOG_WARN(logger_name, ...)                                 \
    if (spdlog::get(logger_name) != nullptr) {                                 \
        SPDLOG_LOGGER_WARN(spdlog::get(logger_name), __VA_ARGS__);             \
    } else {                                                                   \
        INVALID_LOGGER_REQUEST(logger_name)                                    \
        if (spdlog::get(AIDE_DEFAULT_MACRO_LOGGER) != nullptr) {               \
            SPDLOG_LOGGER_WARN(spdlog::get(AIDE_DEFAULT_MACRO_LOGGER),         \
                               __VA_ARGS__);                                   \
        }                                                                      \
    }

// NOLINTNEXTLINE
#define AIDE_LOG_WARN(...)                                                     \
    AIDE_CUSTOM_LOG_WARN(AIDE_DEFAULT_MACRO_LOGGER, __VA_ARGS__);

// NOLINTNEXTLINE
#define AIDE_CUSTOM_LOG_ERROR(logger_name, ...)                                \
    if (spdlog::get(logger_name) != nullptr) {                                 \
        SPDLOG_LOGGER_ERROR(spdlog::get(logger_name), __VA_ARGS__);            \
    } else {                                                                   \
        INVALID_LOGGER_REQUEST(logger_name)                                    \
        if (spdlog::get(AIDE_DEFAULT_MACRO_LOGGER) != nullptr) {               \
            SPDLOG_LOGGER_ERROR(spdlog::get(AIDE_DEFAULT_MACRO_LOGGER),        \
                                __VA_ARGS__);                                  \
        }                                                                      \
    }

// NOLINTNEXTLINE
#define AIDE_LOG_ERROR(...)                                                    \
    AIDE_CUSTOM_LOG_ERROR(AIDE_DEFAULT_MACRO_LOGGER, __VA_ARGS__);

// NOLINTNEXTLINE
#define AIDE_CUSTOM_LOG_CRITICAL(logger_name, ...)                             \
    if (spdlog::get(logger_name) != nullptr) {                                 \
        SPDLOG_LOGGER_CRITICAL(spdlog::get(logger_name), __VA_ARGS__);         \
    } else {                                                                   \
        INVALID_LOGGER_REQUEST(logger_name)                                    \
        if (spdlog::get(AIDE_DEFAULT_MACRO_LOGGER) != nullptr) {               \
            SPDLOG_LOGGER_CRITICAL(spdlog::get(AIDE_DEFAULT_MACRO_LOGGER),     \
                                   __VA_ARGS__);                               \
        }                                                                      \
    }

// NOLINTNEXTLINE
#define AIDE_LOG_CRITICAL(...)                                                 \
    AIDE_CUSTOM_LOG_CRITICAL(AIDE_DEFAULT_MACRO_LOGGER, __VA_ARGS__);

#endif // AIDE_LOG_HELPER_MACROS_HPP
