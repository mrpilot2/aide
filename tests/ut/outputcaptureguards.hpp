#ifndef AIDE_OUTPUT_CAPTURE_GUARDS_HPP
#define AIDE_OUTPUT_CAPTURE_GUARDS_HPP

#include <cstdio>
#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>
#include <streambuf>
#include <string>
#include <utility>

#include <QTemporaryDir>

#include "logger.hpp"

#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#endif

namespace aide::test
{
    namespace detail
    {
        // MSVC deprecates fopen and the POSIX names in favour of fopen_s and
        // the underscored ones.
#ifdef _WIN32
        inline std::FILE* openForWriting(const char* path)
        {
            std::FILE* file{nullptr};
            return fopen_s(&file, path, "w") == 0 ? file : nullptr;
        }
        inline int fdOf(std::FILE* stream)
        {
            return _fileno(stream);
        }
        inline int dupFd(int descriptor)
        {
            return _dup(descriptor);
        }
        inline int dup2Fd(int source, int target)
        {
            return _dup2(source, target);
        }
        inline int closeFd(int descriptor)
        {
            return _close(descriptor);
        }
#else
        inline std::FILE* openForWriting(const char* path)
        {
            // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
            return std::fopen(path, "w");
        }
        inline int fdOf(std::FILE* stream)
        {
            return fileno(stream);
        }
        inline int dupFd(int descriptor)
        {
            return dup(descriptor);
        }
        inline int dup2Fd(int source, int target)
        {
            return dup2(source, target);
        }
        inline int closeFd(int descriptor)
        {
            return close(descriptor);
        }
#endif

        // Redirects the process's stdout file descriptor into a private
        // temporary file for the guard's lifetime. Swapping std::cout's buffer
        // is not enough here: spdlog's console sinks write to the C stdout
        // stream (or, on Windows, the STD_OUTPUT_HANDLE that _dup2 updates for
        // console apps), so only an fd-level redirect catches them. The guard
        // both keeps the test output clean and lets the test assert on what was
        // written.
        class StdoutCaptureGuard
        {
        public:
            StdoutCaptureGuard()
                : m_captureFileName{
                      m_captureDir.filePath(QStringLiteral("stdout.txt"))
                          .toStdString()}
            {
                if (!m_captureDir.isValid()) { return; }
                m_captureFile = openForWriting(m_captureFileName.c_str());
                if (m_captureFile == nullptr) { return; }

                [[maybe_unused]] auto flushed = std::fflush(stdout);
                m_savedStdout = detail::dupFd(detail::fdOf(stdout));
                if (m_savedStdout < 0 ||
                    detail::dup2Fd(detail::fdOf(m_captureFile),
                                   detail::fdOf(stdout)) < 0) {
                    // Leave stdout untouched; isCapturing() reports the
                    // failure.
                    if (m_savedStdout >= 0) { detail::closeFd(m_savedStdout); }
                    m_savedStdout = -1;
                    closeCaptureFile();
                }
            }

            StdoutCaptureGuard(const StdoutCaptureGuard&)            = delete;
            StdoutCaptureGuard& operator=(const StdoutCaptureGuard&) = delete;
            StdoutCaptureGuard(StdoutCaptureGuard&&)                 = delete;
            StdoutCaptureGuard& operator=(StdoutCaptureGuard&&)      = delete;

            ~StdoutCaptureGuard() { restore(); }

            // False if the redirect could not be set up, in which case stdout
            // was never touched and captured() returns nothing.
            [[nodiscard]] bool isCapturing() const
            {
                return m_savedStdout >= 0;
            }

            // Ends the redirect and returns everything written to stdout while
            // it was active.
            [[nodiscard]] std::string captured()
            {
                restore();
                const std::ifstream file(m_captureFileName, std::ios::in);
                std::stringstream content;
                content << file.rdbuf();
                return content.str();
            }

        private:
            void restore()
            {
                if (m_savedStdout < 0) { return; }
                [[maybe_unused]] auto flushed = std::fflush(stdout);
                detail::dup2Fd(m_savedStdout, detail::fdOf(stdout));
                detail::closeFd(m_savedStdout);
                m_savedStdout = -1;
                closeCaptureFile();
            }

            void closeCaptureFile()
            {
                if (m_captureFile == nullptr) { return; }
                // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
                [[maybe_unused]] auto closed = std::fclose(m_captureFile);
                m_captureFile                = nullptr;
            }

            QTemporaryDir m_captureDir;
            std::string m_captureFileName;
            std::FILE* m_captureFile{nullptr};
            int m_savedStdout{-1};
        };

        // Captures everything streamed to std::cerr for the guard's lifetime.
        class CerrCaptureGuard
        {
        public:
            CerrCaptureGuard()
                : m_previous{std::cerr.rdbuf(m_captured.rdbuf())}
            {}

            CerrCaptureGuard(const CerrCaptureGuard&)            = delete;
            CerrCaptureGuard& operator=(const CerrCaptureGuard&) = delete;
            CerrCaptureGuard(CerrCaptureGuard&&)                 = delete;
            CerrCaptureGuard& operator=(CerrCaptureGuard&&)      = delete;

            ~CerrCaptureGuard() { std::cerr.rdbuf(m_previous); }

            [[nodiscard]] std::string captured() const
            {
                return m_captured.str();
            }

        private:
            std::ostringstream m_captured;
            std::streambuf* m_previous;
        };
    } // namespace detail

    // Both capture functions run `action` with the stream redirected and
    // return what it wrote, so a test can assert on a deliberately triggered
    // diagnostic instead of letting it leak into the test output.
    //
    // They take a callable rather than handing out a scoped guard because no
    // Catch2 assertion may run while a capture is active: reporters that
    // capture output (junit on CI) swap std::cout/std::cerr's buffer back to
    // their own every time an assertion is reported, silently ending any
    // redirect a test set up itself. Keep REQUIRE/CHECK out of `action`.

    // Returns std::nullopt if stdout could not be redirected, in which case
    // `action` is not run.
    template <typename Action>
    [[nodiscard]] std::optional<std::string> captureStdout(Action&& action)
    {
        detail::StdoutCaptureGuard guard;
        if (!guard.isCapturing()) { return std::nullopt; }
        std::forward<Action>(action)();
        return guard.captured();
    }

    template <typename Action>
    [[nodiscard]] std::string captureCerr(Action&& action)
    {
        const detail::CerrCaptureGuard guard;
        std::forward<Action>(action)();
        return guard.captured();
    }

    // Turns the debug-build console log sink back on for the guard's
    // lifetime. The test runner disables it for the whole run (see
    // catch_main.cpp), so the guard restores exactly that default.
    class ConsoleLoggingGuard
    {
    public:
        ConsoleLoggingGuard() { aide::Logger::enableLoggingToConsole(); }

        ConsoleLoggingGuard(const ConsoleLoggingGuard&)            = delete;
        ConsoleLoggingGuard& operator=(const ConsoleLoggingGuard&) = delete;
        ConsoleLoggingGuard(ConsoleLoggingGuard&&)                 = delete;
        ConsoleLoggingGuard& operator=(ConsoleLoggingGuard&&)      = delete;

        ~ConsoleLoggingGuard() { aide::Logger::disableLoggingToConsole(); }
    };
} // namespace aide::test

#endif // AIDE_OUTPUT_CAPTURE_GUARDS_HPP
