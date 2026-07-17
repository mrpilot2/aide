#ifndef AIDE_STANDARD_PATHS_TEST_GUARDS_HPP
#define AIDE_STANDARD_PATHS_TEST_GUARDS_HPP

#include <string>
#include <utility>

#include <QByteArray>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QIODevice>
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
        // Creates an empty regular file at the shallowest path segment of
        // targetPath that doesn't yet exist, and returns that file's path
        // (or an empty string if targetPath already fully exists, or the
        // file couldn't be created). A later QDir::mkpath(targetPath) call
        // is then guaranteed to fail: a directory can never be created
        // where a file already sits, regardless of platform-specific
        // permission semantics - unlike removing the owner-write bit,
        // which is a POSIX concept that doesn't reliably stop directory
        // content creation on Windows (a folder's read-only attribute is
        // cosmetic there, not an access control restriction).
        inline QString blockPathWithFile(const QString& targetPath)
        {
            QString path = QDir::cleanPath(targetPath);
            QString shallowestMissing;
            while (!path.isEmpty() && !QFileInfo::exists(path)) {
                shallowestMissing = path;
                const auto parent = QFileInfo(path).path();
                if (parent == path) { break; }
                path = parent;
            }
            if (shallowestMissing.isEmpty()) { return {}; }
            QFile blocker(shallowestMissing);
            if (!blocker.open(QIODevice::WriteOnly)) { return {}; }
            blocker.close();
            return shallowestMissing;
        }
    } // namespace detail

    // Blocks a QStandardPaths location from ever being created for the
    // guard's lifetime, by placing an empty file exactly where Qt would
    // need to create a directory (see blockPathWithFile). Use this instead
    // of an env-var override to isolate CacheLocation in particular: an
    // overridden HOME is not reliable there since Windows resolves it via
    // SHGetKnownFolderPath (ignores env vars entirely) and macOS's
    // NSSearchPathForDirectoriesInDomains-based resolution silently skips
    // its own test-mode substitution once HOME no longer matches what
    // Foundation actually resolved.
    class BlockedStandardLocationGuard
    {
    public:
        explicit BlockedStandardLocationGuard(
            QStandardPaths::StandardLocation location)
            : m_blockerPath(detail::blockPathWithFile(
                  QStandardPaths::writableLocation(location)))
        {}

        BlockedStandardLocationGuard(const BlockedStandardLocationGuard&) =
            delete;
        BlockedStandardLocationGuard& operator=(
            const BlockedStandardLocationGuard&)                     = delete;
        BlockedStandardLocationGuard(BlockedStandardLocationGuard&&) = delete;
        BlockedStandardLocationGuard& operator=(
            BlockedStandardLocationGuard&&) = delete;

        ~BlockedStandardLocationGuard()
        {
            if (!m_blockerPath.isEmpty()) { QFile::remove(m_blockerPath); }
        }

    private:
        QString m_blockerPath;
    };
} // namespace aide::test

#endif // AIDE_STANDARD_PATHS_TEST_GUARDS_HPP
