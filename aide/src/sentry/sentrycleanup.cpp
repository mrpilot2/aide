

#include "sentrycleanup.hpp"

#include "sentry.hpp"

using aide::SentryCleanup;

SentryCleanup::~SentryCleanup()
{
    Sentry::closeConnection();
}
