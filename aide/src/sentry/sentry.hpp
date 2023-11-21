
#ifndef AIDE_SENTRY_HPP
#define AIDE_SENTRY_HPP

#include <string>

#include <aide/applicationconfig.hpp>

namespace aide
{
    class Sentry
    {
    public:
        static void initializeConnection(const SentryConfig& config);

        static void closeConnection();
    };
} // namespace aide

#endif // AIDE_SENTRY_HPP
