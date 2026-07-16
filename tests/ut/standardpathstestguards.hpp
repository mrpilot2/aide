#ifndef AIDE_STANDARD_PATHS_TEST_GUARDS_HPP
#define AIDE_STANDARD_PATHS_TEST_GUARDS_HPP

#include <string>
#include <utility>

#include <QByteArray>
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
} // namespace aide::test

#endif // AIDE_STANDARD_PATHS_TEST_GUARDS_HPP
