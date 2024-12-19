#ifndef AIDE_LOGGER_INTERFACE_HPP
#define AIDE_LOGGER_INTERFACE_HPP

#include <format>
#include <memory>
#include <utility>

#include <string_view>

namespace aide
{
    class LoggerInterface
    {
    public:
        virtual ~LoggerInterface() = default;

        template <typename... Args>
        void trace(const std::format_string<Args...> fmt, Args&&... args) const
        {
            doLogTrace(doFormat(fmt, std::forward<Args>(args)...));
        }

        template <typename... Args>
        void debug(const std::format_string<Args...> fmt, Args&&... args) const
        {
            doLogDebug(doFormat(fmt, std::forward<Args>(args)...));
        }

        template <typename... Args>
        void info(const std::format_string<Args...> fmt, Args&&... args) const
        {
            doLogInfo(doFormat(fmt, std::forward<Args>(args)...));
        }

        template <typename... Args>
        void warn(const std::format_string<Args...> fmt, Args&&... args) const
        {
            doLogWarn(doFormat(fmt, std::forward<Args>(args)...));
        }

        template <typename... Args>
        void error(const std::format_string<Args...> fmt, Args&&... args) const
        {
            doLogError(doFormat(fmt, std::forward<Args>(args)...));
        }

        template <typename... Args>
        void critical(const std::format_string<Args...> fmt,
                      Args&&... args) const
        {
            doLogCritical(doFormat(fmt, std::forward<Args>(args)...));
        }

        virtual void flush() = 0;

    private:
        template <typename... Args>
        static std::string doFormat(const std::format_string<Args...> fmt,
                                    Args&&... args)
        {
            return std::format(fmt, std::forward<Args>(args)...);
        }

        virtual void doLogTrace(std::string_view message) const = 0;

        virtual void doLogDebug(std::string_view message) const = 0;

        virtual void doLogInfo(std::string_view message) const = 0;

        virtual void doLogWarn(std::string_view message) const = 0;

        virtual void doLogError(std::string_view message) const = 0;

        virtual void doLogCritical(std::string_view message) const = 0;
    };

    using LoggerPtr = std::shared_ptr<LoggerInterface>;
} // namespace aide
#endif // AIDE_LOGGER_INTERFACE_HPP
