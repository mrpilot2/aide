#ifndef AIDE_STANDARD_PATHS_TEST_GUARDS_HPP
#define AIDE_STANDARD_PATHS_TEST_GUARDS_HPP

#include <string>
#include <utility>

#include <QByteArray>
#include <QDir>
#include <QFile>
#include <QFileDevice>
#include <QStandardPaths>
#include <QString>

namespace aide::test
{
    // Restores an environment variable to whatever it was before the test
    // touched it, even if a REQUIRE() aborts the test case early.
    class EnvVarGuard
    {
    public:
        EnvVarGuard(std::string name, const QString& value)
            : m_name{std::move(name)}
            , m_hadPrevious{qEnvironmentVariableIsSet(m_name.c_str())}
            , m_previous{qgetenv(m_name.c_str())}
        {
            qputenv(m_name.c_str(), value.toUtf8());
        }

        EnvVarGuard(const EnvVarGuard&)            = delete;
        EnvVarGuard& operator=(const EnvVarGuard&) = delete;
        EnvVarGuard(EnvVarGuard&&)                 = delete;
        EnvVarGuard& operator=(EnvVarGuard&&)      = delete;

        ~EnvVarGuard()
        {
            if (m_hadPrevious) {
                qputenv(m_name.c_str(), m_previous);
            } else {
                qunsetenv(m_name.c_str());
            }
        }

    private:
        std::string m_name;
        bool m_hadPrevious;
        QByteArray m_previous;
    };

    // Qt's standard-paths test mode redirects CacheLocation (and friends)
    // under a sandbox so tests never touch the real user cache directory.
    class StandardPathsTestModeGuard
    {
    public:
        StandardPathsTestModeGuard()
        {
            QStandardPaths::setTestModeEnabled(true);
        }

        StandardPathsTestModeGuard(const StandardPathsTestModeGuard&) = delete;
        StandardPathsTestModeGuard& operator=(
            const StandardPathsTestModeGuard&)                   = delete;
        StandardPathsTestModeGuard(StandardPathsTestModeGuard&&) = delete;
        StandardPathsTestModeGuard& operator=(StandardPathsTestModeGuard&&) =
            delete;

        ~StandardPathsTestModeGuard()
        {
            QStandardPaths::setTestModeEnabled(false);
        }
    };

    namespace detail
    {
        // Finds the "qttest" (Windows) or ".qttest" (generic Unix/macOS)
        // sandbox segment that QStandardPaths::setTestModeEnabled(true)
        // inserts into a resolved location, returning everything up to and
        // including that segment. Returns an empty string if neither marker
        // is present, meaning test mode did not actually sandbox this
        // location on the current platform.
        inline QString findTestModeSandboxRoot(const QString& resolvedLocation)
        {
            for (const auto& marker :
                 {QStringLiteral("/.qttest"), QStringLiteral("/qttest")}) {
                const auto index = resolvedLocation.indexOf(marker);
                if (index != -1) {
                    return resolvedLocation.left(index + marker.length());
                }
            }
            return {};
        }
    } // namespace detail

    // Makes QStandardPaths::CacheLocation's test-mode sandbox directory
    // unwritable for the guard's lifetime, so any code trying to create a
    // subdirectory underneath it is guaranteed to fail - on every platform.
    //
    // An env-var override of HOME (the technique that works for
    // TempLocation via TMPDIR/TMP/TEMP) is not reliable for CacheLocation:
    // Windows resolves it via SHGetKnownFolderPath, which ignores env vars
    // entirely, and macOS's NSSearchPathForDirectoriesInDomains-based
    // resolution silently skips its own test-mode substitution once HOME
    // no longer matches what Foundation actually resolved. Targeting the
    // "qttest"/".qttest" segment Qt itself inserts sidesteps both platform
    // quirks.
    class UnwritableCacheLocationGuard
    {
    public:
        UnwritableCacheLocationGuard()
            : m_path(detail::findTestModeSandboxRoot(
                  QStandardPaths::writableLocation(
                      QStandardPaths::CacheLocation)))
        {
            if (!m_path.isEmpty()) {
                QDir().mkpath(m_path);
                QFile::setPermissions(
                    m_path, QFileDevice::ReadOwner | QFileDevice::ExeOwner);
            }
        }

        UnwritableCacheLocationGuard(const UnwritableCacheLocationGuard&) =
            delete;
        UnwritableCacheLocationGuard& operator=(
            const UnwritableCacheLocationGuard&)                     = delete;
        UnwritableCacheLocationGuard(UnwritableCacheLocationGuard&&) = delete;
        UnwritableCacheLocationGuard& operator=(
            UnwritableCacheLocationGuard&&) = delete;

        ~UnwritableCacheLocationGuard()
        {
            if (!m_path.isEmpty()) {
                QFile::setPermissions(m_path, QFileDevice::ReadOwner |
                                                  QFileDevice::WriteOwner |
                                                  QFileDevice::ExeOwner);
                QDir(m_path).removeRecursively();
            }
        }

    private:
        QString m_path;
    };
} // namespace aide::test

#endif // AIDE_STANDARD_PATHS_TEST_GUARDS_HPP
