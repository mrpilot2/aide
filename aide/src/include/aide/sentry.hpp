
#ifndef AIDE_SENTRY_HPP
#define AIDE_SENTRY_HPP

#include <string>

#include "applicationconfig.hpp"

namespace aide
{
    class Sentry
    {
    public:
        static void initializeConnection(const SentryConfig& config);

        static void closeConnection();

        static void captureException(const std::exception& exception);

        static void captureUnknownException();
    };
} // namespace aide

#endif // AIDE_SENTRY_HPP
