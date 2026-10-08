#include "vcsmanager.hpp"

#include <utility>

#include <QStandardPaths>
#include <QString>

#include <aide/logger/loggerfactory.hpp>

#include "vcsprocessrunner.hpp"
#include "vcsrepository.hpp"

using aide::VcsManager;
using aide::VcsRepositoryInterfacePtr;

namespace
{
    constexpr auto kVcsLoggerName     = "aide.vcs";
    constexpr auto kGitExecutableName = "git";

    std::optional<std::string> findGitOnPath()
    {
        const auto found = QStandardPaths::findExecutable(
            QString::fromLatin1(kGitExecutableName));
        if (found.isEmpty()) { return std::nullopt; }
        return found.toStdString();
    }

    // Resolves the git executable given the current override (if any),
    // shared with every VcsRepository this manager hands out. Captured by
    // shared_ptr rather than by manager `this`, so a repository handle that
    // outlives its VcsManager still resolves safely.
    std::optional<std::string> resolveGitExecutable(
        const std::shared_ptr<std::optional<std::string>>& executableOverride)
    {
        if (executableOverride->has_value()) { return *executableOverride; }
        return findGitOnPath();
    }
} // namespace

VcsManager::VcsManager()
    : m_logger(aide::core::LoggerFactory::createLogger(kVcsLoggerName))
{
    m_runner = std::make_shared<VcsProcessRunner>(m_logger);
}

VcsManager::VcsManager(VcsProcessRunnerInterfacePtr runner, LoggerPtr logger)
    : m_runner(std::move(runner))
    , m_logger(std::move(logger))
{}

std::vector<std::string> VcsManager::registeredRepositories() const
{
    std::vector<std::string> paths;
    paths.reserve(m_repositories.size());
    for (const auto& [path, repository] : m_repositories) {
        paths.push_back(path);
    }
    return paths;
}

VcsRepositoryInterfacePtr VcsManager::addRepository(const std::string& path)
{
    auto repository = std::make_shared<VcsRepository>(
        path, m_runner,
        [executableOverride = m_gitExecutableOverride] {
            return resolveGitExecutable(executableOverride);
        },
        m_logger);

    m_repositories[path] = repository;
    return repository;
}

void VcsManager::setGitExecutablePath(std::string path)
{
    *m_gitExecutableOverride = std::move(path);
}

std::optional<std::string> VcsManager::gitExecutablePath() const
{
    return resolveGitExecutable(m_gitExecutableOverride);
}
