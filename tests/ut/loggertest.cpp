#include <cstdio>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include "application.hpp"
#include "logger.hpp"
#include "outputcaptureguards.hpp"

using aide::FileName;
using aide::Logger;
using aide::LoggerName;
using aide::LogLevel;
using aide::test::captureStdout;
using aide::test::ConsoleLoggingGuard;

namespace
{
    std::string getFileContents(const char* const fileName)
    {
        const std::ifstream logFile(fileName, std::ios::in);
        std::stringstream fileContent;
        fileContent << logFile.rdbuf();
        return fileContent.str();
    }

    void clearLogFile(const char* const fileName)
    {
        std::ofstream file(fileName, std::ios::trunc);
        file << "" << std::flush;
    }
} // namespace

TEST_CASE("Test different log levels", "[Logger]")
{
    const std::string logFileNameStr{TEST_LOG_FILE_LOCATION +
                                     std::string("/aide_test.log")};
    const char* logFileName{logFileNameStr.c_str()};

    [[maybe_unused]] auto res = std::remove(logFileName);

    auto logger = Logger(FileName(logFileName));

#if SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_TRACE

    SECTION(" trace")
    {
        logger.trace("This logs to the file");
        logger.flush();

        REQUIRE_THAT(::getFileContents(logFileName),
                     Catch::Matchers::ContainsSubstring("trace"));
    }

    SECTION(" trace")
    {
        logger.debug("This logs to the file");
        logger.flush();

        REQUIRE_THAT(::getFileContents(logFileName),
                     Catch::Matchers::ContainsSubstring("debug"));
    }
#endif

#if SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_INFO

    SECTION(" info")
    {
        logger.info("This logs to the file");
        logger.flush();

        REQUIRE_THAT(::getFileContents(logFileName),
                     Catch::Matchers::ContainsSubstring("info"));
    }
#endif

#if SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_WARN

    SECTION(" warning")
    {
        logger.warn("This logs to the file");
        logger.flush();

        REQUIRE_THAT(::getFileContents(logFileName),
                     Catch::Matchers::ContainsSubstring("warn"));
    }
#endif

#if SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_ERROR

    SECTION(" error")
    {
        logger.error("This logs to the file");
        logger.flush();

        REQUIRE_THAT(::getFileContents(logFileName),
                     Catch::Matchers::ContainsSubstring("error"));
    }

    SECTION(" critical")
    {
        logger.critical("This logs to the file");
        logger.flush();

        REQUIRE_THAT(::getFileContents(logFileName),
                     Catch::Matchers::ContainsSubstring("critical"));
    }
#endif

    [[maybe_unused]] auto res1 = std::remove(logFileName);
}

#ifndef BUILD_SHARED_LIBS_ON_MSVC
// NOLINTNEXTLINE
TEST_CASE("Test log macros", "[Logger]")
{
    spdlog::drop_all();
    const std::string logFileNameStr =
        TEST_LOG_FILE_LOCATION + std::string("/aide_test.log");
    const char* logFileName{logFileNameStr.c_str()};

    [[maybe_unused]] auto res = std::remove(logFileName);

    auto logger = Logger(FileName(logFileName));

    aide::Application::disableLoggingToConsole();

#if SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_TRACE
    SECTION(" trace default logger")
    {
        AIDE_LOG_TRACE("Test")
        logger.flush();

        REQUIRE_THAT(::getFileContents(logFileName),
                     Catch::Matchers::ContainsSubstring("aide_macro"));
        REQUIRE_THAT(::getFileContents(logFileName),
                     Catch::Matchers::ContainsSubstring("trace"));
    }

    SECTION(" trace available logger")
    {
        Logger custom_logger(FileName(logFileName),
                             LoggerName("my_fancy_logger"));

        AIDE_CUSTOM_LOG_TRACE("my_fancy_logger", "Test")
        custom_logger.flush();

        REQUIRE_THAT(::getFileContents(logFileName),
                     Catch::Matchers::ContainsSubstring("my_fancy_logger -"));
        REQUIRE_THAT(::getFileContents(logFileName),
                     Catch::Matchers::ContainsSubstring("trace"));
    }

    SECTION(" trace unavailable logger")
    {
        AIDE_CUSTOM_LOG_TRACE("my_fancy_logger", "Test")
        logger.flush();

        REQUIRE_THAT(::getFileContents(logFileName),
                     !Catch::Matchers::ContainsSubstring("my_fancy_logger -"));
        REQUIRE_THAT(::getFileContents(logFileName),
                     Catch::Matchers::ContainsSubstring("error"));
        REQUIRE_THAT(::getFileContents(logFileName),
                     Catch::Matchers::ContainsSubstring("trying to access"));
    }
#endif

#if SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_DEBUG

    SECTION(" debug default logger")
    {
        AIDE_LOG_DEBUG("Test")
        logger.flush();

        REQUIRE_THAT(::getFileContents(logFileName),
                     Catch::Matchers::ContainsSubstring("aide_macro"));
        REQUIRE_THAT(::getFileContents(logFileName),
                     Catch::Matchers::ContainsSubstring("debug"));
    }

    SECTION(" debug available logger")
    {
        Logger custom_logger(FileName(logFileName),
                             LoggerName("my_fancy_logger"));

        AIDE_CUSTOM_LOG_DEBUG("my_fancy_logger", "Test")
        custom_logger.flush();

        REQUIRE_THAT(::getFileContents(logFileName),
                     Catch::Matchers::ContainsSubstring("my_fancy_logger -"));
        REQUIRE_THAT(::getFileContents(logFileName),
                     Catch::Matchers::ContainsSubstring("debug"));
    }

    SECTION(" debug unavailable logger")
    {
        AIDE_CUSTOM_LOG_DEBUG("my_fancy_logger", "Test")
        logger.flush();

        REQUIRE_THAT(::getFileContents(logFileName),
                     !Catch::Matchers::ContainsSubstring("my_fancy_logger -"));
        REQUIRE_THAT(::getFileContents(logFileName),
                     Catch::Matchers::ContainsSubstring("error"));
        REQUIRE_THAT(::getFileContents(logFileName),
                     Catch::Matchers::ContainsSubstring("trying to access"));
    }
#endif

#if SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_INFO

    SECTION(" info default logger")
    {
        AIDE_LOG_INFO("Test")
        logger.flush();

        REQUIRE_THAT(::getFileContents(logFileName),
                     Catch::Matchers::ContainsSubstring("aide_macro"));
        REQUIRE_THAT(::getFileContents(logFileName),
                     Catch::Matchers::ContainsSubstring("info"));
    }

    SECTION(" info available logger")
    {
        Logger custom_logger(FileName(logFileName),
                             LoggerName("my_fancy_logger"));

        AIDE_CUSTOM_LOG_INFO("my_fancy_logger", "Test")
        custom_logger.flush();

        REQUIRE_THAT(::getFileContents(logFileName),
                     Catch::Matchers::ContainsSubstring("my_fancy_logger -"));
        REQUIRE_THAT(::getFileContents(logFileName),
                     Catch::Matchers::ContainsSubstring("info"));
    }

    SECTION(" info unavailable logger")
    {
        AIDE_CUSTOM_LOG_INFO("my_fancy_logger", "Test")
        logger.flush();

        REQUIRE_THAT(::getFileContents(logFileName),
                     !Catch::Matchers::ContainsSubstring("my_fancy_logger -"));
        REQUIRE_THAT(::getFileContents(logFileName),
                     Catch::Matchers::ContainsSubstring("error"));
        REQUIRE_THAT(::getFileContents(logFileName),
                     Catch::Matchers::ContainsSubstring("trying to access"));
    }

#endif

#if SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_WARN

    SECTION(" warn default logger")
    {
        AIDE_LOG_WARN("Test")
        logger.flush();

        REQUIRE_THAT(::getFileContents(logFileName),
                     Catch::Matchers::ContainsSubstring("aide_macro"));
        REQUIRE_THAT(::getFileContents(logFileName),
                     Catch::Matchers::ContainsSubstring("warn"));
    }

    SECTION(" warn available logger")
    {
        Logger custom_logger(FileName(logFileName),
                             LoggerName("my_fancy_logger"));

        AIDE_CUSTOM_LOG_WARN("my_fancy_logger", "Test")
        custom_logger.flush();

        REQUIRE_THAT(::getFileContents(logFileName),
                     Catch::Matchers::ContainsSubstring("my_fancy_logger -"));
        REQUIRE_THAT(::getFileContents(logFileName),
                     Catch::Matchers::ContainsSubstring("warn"));
    }

    SECTION(" warn unavailable logger")
    {
        AIDE_CUSTOM_LOG_WARN("my_fancy_logger", "Test")
        logger.flush();

        REQUIRE_THAT(::getFileContents(logFileName),
                     !Catch::Matchers::ContainsSubstring("my_fancy_logger -"));
        REQUIRE_THAT(::getFileContents(logFileName),
                     Catch::Matchers::ContainsSubstring("error"));
        REQUIRE_THAT(::getFileContents(logFileName),
                     Catch::Matchers::ContainsSubstring("trying to access"));
    }
#endif

#if SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_ERROR

    SECTION(" error default logger")
    {
        AIDE_LOG_ERROR("Test info")
        logger.flush();

        REQUIRE_THAT(::getFileContents(logFileName),
                     Catch::Matchers::ContainsSubstring("aide_macro"));
        REQUIRE_THAT(::getFileContents(logFileName),
                     Catch::Matchers::ContainsSubstring("error"));
    }

    SECTION(" error available logger")
    {
        Logger custom_logger(FileName(logFileName),
                             LoggerName("my_fancy_logger"));

        AIDE_CUSTOM_LOG_ERROR("my_fancy_logger", "Test")
        custom_logger.flush();

        REQUIRE_THAT(::getFileContents(logFileName),
                     Catch::Matchers::ContainsSubstring("my_fancy_logger -"));
        REQUIRE_THAT(::getFileContents(logFileName),
                     Catch::Matchers::ContainsSubstring("error"));
    }

    SECTION(" error unavailable logger")
    {
        AIDE_CUSTOM_LOG_ERROR("my_fancy_logger", "Test")
        logger.flush();

        REQUIRE_THAT(::getFileContents(logFileName),
                     !Catch::Matchers::ContainsSubstring("my_fancy_logger -"));
        REQUIRE_THAT(::getFileContents(logFileName),
                     Catch::Matchers::ContainsSubstring("error"));
        REQUIRE_THAT(::getFileContents(logFileName),
                     Catch::Matchers::ContainsSubstring("trying to access"));
    }

#endif

#if SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_CRITICAL

    SECTION(" critical default logger")
    {
        AIDE_LOG_CRITICAL("Test")
        logger.flush();

        REQUIRE_THAT(::getFileContents(logFileName),
                     Catch::Matchers::ContainsSubstring("aide_macro"));
        REQUIRE_THAT(::getFileContents(logFileName),
                     Catch::Matchers::ContainsSubstring("critical"));
    }

    SECTION(" critical available logger")
    {
        Logger custom_logger(FileName(logFileName),
                             LoggerName("my_fancy_logger"));

        AIDE_CUSTOM_LOG_CRITICAL("my_fancy_logger", "Test")
        custom_logger.flush();

        REQUIRE_THAT(::getFileContents(logFileName),
                     Catch::Matchers::ContainsSubstring("my_fancy_logger -"));
        REQUIRE_THAT(::getFileContents(logFileName),
                     Catch::Matchers::ContainsSubstring("critical"));
    }

    SECTION(" critical unavailable logger")
    {
        AIDE_CUSTOM_LOG_CRITICAL("my_fancy_logger", "Test")
        logger.flush();

        REQUIRE_THAT(::getFileContents(logFileName),
                     !Catch::Matchers::ContainsSubstring("my_fancy_logger -"));
        REQUIRE_THAT(::getFileContents(logFileName),
                     Catch::Matchers::ContainsSubstring("error"));
        REQUIRE_THAT(::getFileContents(logFileName),
                     Catch::Matchers::ContainsSubstring("trying to access"));
    }
#endif

    ::clearLogFile(logFileName);
}
#endif

#if !defined(NDEBUG) && SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_CRITICAL
TEST_CASE("Logger mirrors messages to the console in debug builds", "[Logger]")
{
    // The test runner disables console logging for the whole run so the
    // debug-build console sink doesn't flood the test output. Re-enable it
    // here and capture stdout to check what the sink writes. Critical is the
    // one level every SPDLOG_ACTIVE_LEVEL short of OFF compiles in.
    const std::string logFileNameStr{TEST_LOG_FILE_LOCATION +
                                     std::string("/aide_console_test.log")};
    [[maybe_unused]] auto res = std::remove(logFileNameStr.c_str());
    spdlog::drop_all();

    const ConsoleLoggingGuard consoleGuard;
    // Construct the logger inside the capture: on Windows the console sink
    // picks up the stdout handle when it is created, not when it writes.
    const auto consoleOutput = captureStdout([&logFileNameStr] {
        auto logger = Logger(FileName(logFileNameStr));
        logger.critical("console sink probe");
        logger.flush();
    });

    REQUIRE(consoleOutput.has_value());

    // NOLINTNEXTLINE(bugprone-unchecked-optional-access)
    REQUIRE_THAT(consoleOutput.value(),
                 Catch::Matchers::ContainsSubstring("console sink probe"));
    REQUIRE_THAT(::getFileContents(logFileNameStr.c_str()),
                 Catch::Matchers::ContainsSubstring("console sink probe"));

    [[maybe_unused]] auto res1 = std::remove(logFileNameStr.c_str());
}
#endif

TEST_CASE("Logger constructor variants", "[Logger]")
{
    // The default and LoggerName constructors hardcode FileName("aide.log"),
    // which the rotating sink writes relative to the current working directory.
    // Run from the build dir so the stray file never lands in the repo root.
    const std::filesystem::path previousWorkingDir{
        std::filesystem::current_path()};
    std::filesystem::current_path(TEST_LOG_FILE_LOCATION);

    SECTION("default constructor constructs without crash")
    {
        spdlog::drop_all();
        Logger logger;
        logger.info("default ctor");
        logger.flush();
    }

    SECTION("LoggerName constructor constructs without crash")
    {
        spdlog::drop_all();
        Logger logger(LoggerName("test_named_logger"));
        logger.info("named ctor");
        logger.flush();
    }

    std::filesystem::current_path(previousWorkingDir);
    [[maybe_unused]] auto res =
        std::remove((std::filesystem::path{TEST_LOG_FILE_LOCATION} / "aide.log")
                        .string()
                        .c_str());
}

#if SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_INFO
// NOLINTNEXTLINE
TEST_CASE("Logger setLevel filters messages", "[Logger]")
{
    const std::string logFileNameStr{TEST_LOG_FILE_LOCATION +
                                     std::string("/aide_setlevel_test.log")};
    const char* logFileName{logFileNameStr.c_str()};
    [[maybe_unused]] auto res = std::remove(logFileName);
    spdlog::drop_all();

    auto logger = Logger(FileName(logFileName));

    SECTION("setLevel Error suppresses Info messages")
    {
        logger.setLevel(LogLevel::Error);
        logger.info("suppressed info message");
        logger.flush();

        REQUIRE_THAT(::getFileContents(logFileName),
                     !Catch::Matchers::ContainsSubstring("suppressed info"));
    }

    SECTION("setLevel Info allows Info messages through")
    {
        logger.setLevel(LogLevel::Info);
        logger.info("visible info message");
        logger.flush();

        REQUIRE_THAT(::getFileContents(logFileName),
                     Catch::Matchers::ContainsSubstring("visible info"));
    }

    SECTION("raising then lowering level re-enables messages")
    {
        logger.setLevel(LogLevel::Error);
        logger.info("first info");
        logger.setLevel(LogLevel::Info);
        logger.info("second info");
        logger.flush();

        REQUIRE_THAT(::getFileContents(logFileName),
                     !Catch::Matchers::ContainsSubstring("first info"));
        REQUIRE_THAT(::getFileContents(logFileName),
                     Catch::Matchers::ContainsSubstring("second info"));
    }

    [[maybe_unused]] auto res2 = std::remove(logFileName);
}
#endif