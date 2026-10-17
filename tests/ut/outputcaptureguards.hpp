#ifndef AIDE_OUTPUT_CAPTURE_GUARDS_HPP
#define AIDE_OUTPUT_CAPTURE_GUARDS_HPP

#include <cstdio>
#include <fstream>
#include <iostream>
#include <sstream>
#include <streambuf>
#include <string>
#include <utility>

#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#endif

namespace aide::test
{
    namespace detail
    {
        // MSVC deprecates the POSIX names in favour of the underscored ones.
#ifdef _WIN32
        inline int fdOf(std::FILE* stream)
        {
            return _fileno(stream);
        }
        inline int dupFd(int fd)
        {
            return _dup(fd);
        }
        inline int dup2Fd(int from, int to)
        {
            return _dup2(from, to);
        }
        inline int closeFd(int fd)
        {
            return _close(fd);
        }
#else
        inline int fdOf(std::FILE* stream)
        {
            return fileno(stream);
        }
        inline int dupFd(int fd)
        {
            return dup(fd);
        }
        inline int dup2Fd(int from, int to)
        {
            return dup2(from, to);
        }
        inline int closeFd(int fd)
        {
            return close(fd);
        }
#endif
    } // namespace detail

    // Redirects the process's stdout file descriptor into a file for the
    // guard's lifetime. Swapping std::cout's buffer is not enough here:
    // spdlog's console sinks write to the C stdout stream (or, on Windows,
    // the STD_OUTPUT_HANDLE that _dup2 updates for console apps), so only an
    // fd-level redirect catches them. The guard both keeps the test output
    // clean and lets the test assert on what was written.
    class StdoutCaptureGuard
    {
    public:
        explicit StdoutCaptureGuard(std::string captureFileName)
            : m_captureFileName{std::move(captureFileName)}
            // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
            , m_captureFile{std::fopen(m_captureFileName.c_str(), "w")}
        {
            if (m_captureFile == nullptr) { return; }
            [[maybe_unused]] auto flushed = std::fflush(stdout);
            m_savedStdout                 = detail::dupFd(detail::fdOf(stdout));
            detail::dup2Fd(detail::fdOf(m_captureFile), detail::fdOf(stdout));
        }

        StdoutCaptureGuard(const StdoutCaptureGuard&)            = delete;
        StdoutCaptureGuard& operator=(const StdoutCaptureGuard&) = delete;
        StdoutCaptureGuard(StdoutCaptureGuard&&)                 = delete;
        StdoutCaptureGuard& operator=(StdoutCaptureGuard&&)      = delete;

        ~StdoutCaptureGuard()
        {
            restore();
            [[maybe_unused]] auto res = std::remove(m_captureFileName.c_str());
        }

        // Ends the redirect and returns everything written to stdout while it
        // was active.
        std::string captured()
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
            if (m_captureFile == nullptr) { return; }
            [[maybe_unused]] auto flushed = std::fflush(stdout);
            detail::dup2Fd(m_savedStdout, detail::fdOf(stdout));
            detail::closeFd(m_savedStdout);
            // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
            [[maybe_unused]] auto closed = std::fclose(m_captureFile);
            m_captureFile                = nullptr;
        }

        std::string m_captureFileName;
        std::FILE* m_captureFile{nullptr};
        int m_savedStdout{-1};
    };

    // Captures everything streamed to std::cerr for the guard's lifetime, so
    // a test that deliberately triggers a diagnostic can assert on it instead
    // of letting it leak into the test output.
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

        [[nodiscard]] std::string captured() const { return m_captured.str(); }

    private:
        std::ostringstream m_captured;
        std::streambuf* m_previous;
    };
} // namespace aide::test

#endif // AIDE_OUTPUT_CAPTURE_GUARDS_HPP
