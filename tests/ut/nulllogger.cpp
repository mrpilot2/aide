#include "nulllogger.hpp"

void aide::test::NullLogger::flush() {}

void aide::test::NullLogger::doLogTrace(
    [[maybe_unused]] std::string_view message) const
{}
void aide::test::NullLogger::doLogDebug(
    [[maybe_unused]] std::string_view message) const
{}
void aide::test::NullLogger::doLogInfo(
    [[maybe_unused]] std::string_view message) const
{}
void aide::test::NullLogger::doLogWarn(
    [[maybe_unused]] std::string_view message) const
{}
void aide::test::NullLogger::doLogError(
    [[maybe_unused]] std::string_view message) const
{}
void aide::test::NullLogger::doLogCritical(
    [[maybe_unused]] std::string_view message) const
{}
