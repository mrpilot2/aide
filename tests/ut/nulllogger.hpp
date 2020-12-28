#ifndef AIDE_NULL_LOGGER_HPP
#define AIDE_NULL_LOGGER_HPP

#include <aide/loggerinterface.hpp>

namespace aide::test
{
    class NullLogger : public LoggerInterface
    {
    public:
        void flush() override;

    private:
        void doLogTrace(std::string_view message) const override;
        void doLogDebug(std::string_view message) const override;
        void doLogInfo(std::string_view message) const override;
        void doLogWarn(std::string_view message) const override;
        void doLogError(std::string_view message) const override;
        void doLogCritical(std::string_view message) const override;
    };
} // namespace aide::test

#endif // AIDE_NULL_LOGGER_HPP
