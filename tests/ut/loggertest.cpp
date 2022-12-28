#include <cstdio>
#include <fstream>
#include <sstream>

#include <catch2/catch.hpp>

#include "logger.hpp"

using aide::FileName;
using aide::Logger;
using aide::LoggerName;

namespace
{
    size_t lookForContentInFile(const char* const fileName,
                                const char* const searchString)
    {
        std::ifstream logFile(fileName, std::ios::in);
        std::stringstream fileContent;
        fileContent << logFile.rdbuf();
        return fileContent.str().find(searchString);
    }
} // namespace

TEST_CASE("Test construction of logger", "[Logger]")
{
    Logger logger;
}

TEST_CASE("Test different log levels", "[Logger]")
{
    const char* const logFileName = "aide_test.log";

    std::remove(logFileName);

    Logger logger = Logger(FileName(logFileName));

#if SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_TRACE

    SECTION(" trace")
    {
        logger.trace("This logs to the file");
        logger.flush();

        REQUIRE(::lookForContentInFile(logFileName, "trace") !=
                std::string::npos);
    }

    SECTION(" trace")
    {
        logger.debug("This logs to the file");
        logger.flush();

        REQUIRE(::lookForContentInFile(logFileName, "debug") !=
                std::string::npos);
    }
#endif

#if SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_INFO

    SECTION(" info")
    {
        logger.info("This logs to the file");
        logger.flush();

        REQUIRE(::lookForContentInFile(logFileName, "info") !=
                std::string::npos);
    }
#endif

#if SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_WARN

    SECTION(" warning")
    {
        logger.warn("This logs to the file");
        logger.flush();

        REQUIRE(::lookForContentInFile(logFileName, "warn") !=
                std::string::npos);
    }
#endif

#if SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_ERROR

    SECTION(" error")
    {
        logger.error("This logs to the file");
        logger.flush();

        REQUIRE(::lookForContentInFile(logFileName, "error") !=
                std::string::npos);
    }

    SECTION(" critical")
    {
        logger.critical("This logs to the file");
        logger.flush();

        REQUIRE(::lookForContentInFile(logFileName, "critical") !=
                std::string::npos);
    }
#endif
}

TEST_CASE("Test log macros", "[Logger]")
{
    spdlog::drop_all();
    const char* const logFileName = "aide_test.log";

    std::remove(logFileName);

    Logger logger = Logger(FileName(logFileName));

#if SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_TRACE
    SECTION(" trace default logger")
    {
        AIDE_LOG_TRACE("Test")
        logger.flush();

        REQUIRE(::lookForContentInFile(logFileName, "aide_macro") !=
                std::string::npos);
        REQUIRE(::lookForContentInFile(logFileName, "trace") !=
                std::string::npos);
    }

    SECTION(" trace available logger")
    {
        Logger custom_logger(FileName(logFileName),
                             LoggerName("my_fancy_logger"));

        AIDE_CUSTOM_LOG_TRACE("my_fancy_logger", "Test")
        custom_logger.flush();

        REQUIRE(::lookForContentInFile(logFileName, "my_fancy_logger -") !=
                std::string::npos);
        REQUIRE(::lookForContentInFile(logFileName, "trace") !=
                std::string::npos);
    }

    SECTION(" trace unavailable logger")
    {
        AIDE_CUSTOM_LOG_TRACE("my_fancy_logger", "Test")
        logger.flush();

        REQUIRE(::lookForContentInFile(logFileName, "my_fancy_logger -") ==
                std::string::npos);
        REQUIRE(::lookForContentInFile(logFileName, "error") !=
                std::string::npos);
        REQUIRE(::lookForContentInFile(logFileName, "trying to access") !=
                std::string::npos);
    }
#endif

#if SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_DEBUG

    SECTION(" debug default logger")
    {
        AIDE_LOG_DEBUG("Test")
        logger.flush();

        REQUIRE(::lookForContentInFile(logFileName, "aide_macro") !=
                std::string::npos);
        REQUIRE(::lookForContentInFile(logFileName, "debug") !=
                std::string::npos);
    }

    SECTION(" debug available logger")
    {
        Logger custom_logger(FileName(logFileName),
                             LoggerName("my_fancy_logger"));

        AIDE_CUSTOM_LOG_DEBUG("my_fancy_logger", "Test")
        custom_logger.flush();

        REQUIRE(::lookForContentInFile(logFileName, "my_fancy_logger -") !=
                std::string::npos);
        REQUIRE(::lookForContentInFile(logFileName, "debug") !=
                std::string::npos);
    }

    SECTION(" debug unavailable logger")
    {
        AIDE_CUSTOM_LOG_DEBUG("my_fancy_logger", "Test")
        logger.flush();

        REQUIRE(::lookForContentInFile(logFileName, "my_fancy_logger -") ==
                std::string::npos);
        REQUIRE(::lookForContentInFile(logFileName, "error") !=
                std::string::npos);
        REQUIRE(::lookForContentInFile(logFileName, "trying to access") !=
                std::string::npos);
    }
#endif

#if SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_INFO

    SECTION(" info default logger")
    {
        AIDE_LOG_INFO("Test")
        logger.flush();

        REQUIRE(::lookForContentInFile(logFileName, "aide_macro") !=
                std::string::npos);
        REQUIRE(::lookForContentInFile(logFileName, "info") !=
                std::string::npos);
    }

    SECTION(" info available logger")
    {
        Logger custom_logger(FileName(logFileName),
                             LoggerName("my_fancy_logger"));

        AIDE_CUSTOM_LOG_INFO("my_fancy_logger", "Test")
        custom_logger.flush();

        REQUIRE(::lookForContentInFile(logFileName, "my_fancy_logger -") !=
                std::string::npos);
        REQUIRE(::lookForContentInFile(logFileName, "info") !=
                std::string::npos);
    }

    SECTION(" info unavailable logger")
    {
        AIDE_CUSTOM_LOG_INFO("my_fancy_logger", "Test")
        logger.flush();

        REQUIRE(::lookForContentInFile(logFileName, "my_fancy_logger -") ==
                std::string::npos);
        REQUIRE(::lookForContentInFile(logFileName, "error") !=
                std::string::npos);
        REQUIRE(::lookForContentInFile(logFileName, "trying to access") !=
                std::string::npos);
    }

#endif

#if SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_WARN

    SECTION(" warn default logger")
    {
        AIDE_LOG_WARN("Test")
        logger.flush();

        REQUIRE(::lookForContentInFile(logFileName, "aide_macro") !=
                std::string::npos);
        REQUIRE(::lookForContentInFile(logFileName, "warn") !=
                std::string::npos);
    }

    SECTION(" warn available logger")
    {
        Logger custom_logger(FileName(logFileName),
                             LoggerName("my_fancy_logger"));

        AIDE_CUSTOM_LOG_WARN("my_fancy_logger", "Test")
        custom_logger.flush();

        REQUIRE(::lookForContentInFile(logFileName, "my_fancy_logger -") !=
                std::string::npos);
        REQUIRE(::lookForContentInFile(logFileName, "warn") !=
                std::string::npos);
    }

    SECTION(" warn unavailable logger")
    {
        AIDE_CUSTOM_LOG_WARN("my_fancy_logger", "Test")
        logger.flush();

        REQUIRE(::lookForContentInFile(logFileName, "my_fancy_logger -") ==
                std::string::npos);
        REQUIRE(::lookForContentInFile(logFileName, "error") !=
                std::string::npos);
        REQUIRE(::lookForContentInFile(logFileName, "trying to access") !=
                std::string::npos);
    }
#endif

#if SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_ERROR

    SECTION(" error default logger")
    {
        AIDE_LOG_ERROR("Test info")
        logger.flush();

        REQUIRE(::lookForContentInFile(logFileName, "aide_macro") !=
                std::string::npos);
        REQUIRE(::lookForContentInFile(logFileName, "error") !=
                std::string::npos);
    }

    SECTION(" error available logger")
    {
        Logger custom_logger(FileName(logFileName),
                             LoggerName("my_fancy_logger"));

        AIDE_CUSTOM_LOG_ERROR("my_fancy_logger", "Test")
        custom_logger.flush();

        REQUIRE(::lookForContentInFile(logFileName, "my_fancy_logger -") !=
                std::string::npos);
        REQUIRE(::lookForContentInFile(logFileName, "error") !=
                std::string::npos);
    }

    SECTION(" error unavailable logger")
    {
        AIDE_CUSTOM_LOG_ERROR("my_fancy_logger", "Test")
        logger.flush();

        REQUIRE(::lookForContentInFile(logFileName, "my_fancy_logger -") ==
                std::string::npos);
        REQUIRE(::lookForContentInFile(logFileName, "error") !=
                std::string::npos);
        REQUIRE(::lookForContentInFile(logFileName, "trying to access") !=
                std::string::npos);
    }

#endif

#if SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_CRITICAL

    SECTION(" critical default logger")
    {
        AIDE_LOG_CRITICAL("Test")
        logger.flush();

        REQUIRE(::lookForContentInFile(logFileName, "aide_macro") !=
                std::string::npos);
        REQUIRE(::lookForContentInFile(logFileName, "critical") !=
                std::string::npos);
    }

    SECTION(" critical available logger")
    {
        Logger custom_logger(FileName(logFileName),
                             LoggerName("my_fancy_logger"));

        AIDE_CUSTOM_LOG_CRITICAL("my_fancy_logger", "Test")
        custom_logger.flush();

        REQUIRE(::lookForContentInFile(logFileName, "my_fancy_logger -") !=
                std::string::npos);
        REQUIRE(::lookForContentInFile(logFileName, "critical") !=
                std::string::npos);
    }

    SECTION(" critical unavailable logger")
    {
        AIDE_CUSTOM_LOG_CRITICAL("my_fancy_logger", "Test")
        logger.flush();

        REQUIRE(::lookForContentInFile(logFileName, "my_fancy_logger -") ==
                std::string::npos);
        REQUIRE(::lookForContentInFile(logFileName, "error") !=
                std::string::npos);
        REQUIRE(::lookForContentInFile(logFileName, "trying to access") !=
                std::string::npos);
    }
#endif
}
