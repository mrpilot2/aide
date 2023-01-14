

#include "sentrycleanup.hpp"

#include "aide/sentry.hpp"

using aide::SentryCleanup;

SentryCleanup::~SentryCleanup()
{
    Sentry::closeConnection();
}
