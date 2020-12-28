#ifndef AIDE_LOGGER_INTERFACE_HPP
#define AIDE_LOGGER_INTERFACE_HPP

#include <fmt/format.h>

namespace aide
{
    class LoggerInterface
    {
    public:
        virtual ~LoggerInterface() = default;

        template <typename... Args>
        void trace(std::string_view fmt, const Args&... args)
        {
            doLogTrace(doFormat(fmt, args...));
        }

        template <typename... Args>
        void debug(std::string_view fmt, const Args&... args)
        {
            doLogDebug(doFormat(fmt, args...));
        }

        template <typename... Args>
        void info(std::string_view fmt, const Args&... args)
        {
            doLogInfo(doFormat(fmt, args...));
        }

        template <typename... Args>
        void warn(std::string_view fmt, const Args&... args)
        {
            doLogWarn(doFormat(fmt, args...));
        }

        template <typename... Args>
        void error(std::string_view fmt, const Args&... args)
        {
            doLogError(doFormat(fmt, args...));
        }

        template <typename... Args>
        void critical(std::string_view fmt, const Args&... args)
        {
            doLogCritical(doFormat(fmt, args...));
        }

        virtual void flush() = 0;

    private:
        template <typename... Args>
        std::string doFormat(std::string_view fmt, const Args&... args)
        {
            return fmt::format(fmt, args...);
        }

        virtual void doLogTrace(std::string_view message) = 0;

        virtual void doLogDebug(std::string_view message) = 0;

        virtual void doLogInfo(std::string_view message) = 0;

        virtual void doLogWarn(std::string_view message) = 0;

        virtual void doLogError(std::string_view message) = 0;

        virtual void doLogCritical(std::string_view message) = 0;
    };
} // namespace aide
#endif // AIDE_LOGGER_INTERFACE_HPP
