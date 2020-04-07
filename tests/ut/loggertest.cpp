#include <cstdio>
#include <fstream>
#include <sstream>

#include <catch2/catch.hpp>

#include <aide/logger.hpp>

using aide::Logger;

namespace
{
    size_t lookForLogLevelInFile(const char* const fileName,
                                 const char* const logLevel)
    {
        std::ifstream logFile(fileName, std::ios::in);
        std::stringstream content;
        content << logFile.rdbuf();
        return content.str().find(logLevel);
    }
} // namespace

TEST_CASE("Test construction of logger")
{
    Logger logger;
}

TEST_CASE("Test different log levels")
{
    const char* const logFileName = "aide_test.log";

    std::remove(logFileName);

    Logger logger(logFileName);

    SECTION(" trace")
    {
        logger.trace("This logs to the file");
        logger.flush();

        REQUIRE(::lookForLogLevelInFile(logFileName, "trace") !=
                std::string::npos);
    }

    SECTION(" trace")
    {
        logger.debug("This logs to the file");
        logger.flush();

        REQUIRE(::lookForLogLevelInFile(logFileName, "debug") !=
                std::string::npos);
    }

    SECTION(" info")
    {
        logger.info("This logs to the file");
        logger.flush();

        REQUIRE(::lookForLogLevelInFile(logFileName, "info") !=
                std::string::npos);
    }

    SECTION(" warning")
    {
        logger.warn("This logs to the file");
        logger.flush();

        REQUIRE(::lookForLogLevelInFile(logFileName, "warn") !=
                std::string::npos);
    }

    SECTION(" error")
    {
        logger.error("This logs to the file");
        logger.flush();

        REQUIRE(::lookForLogLevelInFile(logFileName, "error") !=
                std::string::npos);
    }

    SECTION(" critical")
    {
        logger.critical("This logs to the file");
        logger.flush();

        REQUIRE(::lookForLogLevelInFile(logFileName, "critical") !=
                std::string::npos);
    }
}