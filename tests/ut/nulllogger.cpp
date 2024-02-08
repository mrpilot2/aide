#include "nulllogger.hpp"

using aide::test::NullLogger;

void NullLogger::flush() {}

void NullLogger::doLogTrace([[maybe_unused]] std::string_view message) const {}
void NullLogger::doLogDebug([[maybe_unused]] std::string_view message) const {}
void NullLogger::doLogInfo([[maybe_unused]] std::string_view message) const {}
void NullLogger::doLogWarn([[maybe_unused]] std::string_view message) const {}
void NullLogger::doLogError([[maybe_unused]] std::string_view message) const {}
void NullLogger::doLogCritical([[maybe_unused]] std::string_view message) const
{}
