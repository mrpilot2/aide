#include "vcsrepository.hpp"

#include <sstream>
#include <utility>

using aide::VcsError;
using aide::VcsFileState;
using aide::VcsFileStatus;
using aide::VcsOperationResult;
using aide::VcsProcessResult;
using aide::VcsRepository;
using aide::VcsStatusResult;

namespace
{
    constexpr std::size_t kPorcelainPathOffset = 3; // "XY " prefix

    VcsFileState toFileState(char statusChar)
    {
        switch (statusChar) {
        case 'M':
            return VcsFileState::Modified;
        case 'A':
            return VcsFileState::Added;
        case 'D':
            return VcsFileState::Deleted;
        case 'R':
            return VcsFileState::Renamed;
        case 'C':
            return VcsFileState::Copied;
        case 'U':
            return VcsFileState::Conflicted;
        case '?':
            return VcsFileState::Untracked;
        case '!':
            return VcsFileState::Ignored;
        default:
            return VcsFileState::Unmodified;
        }
    }

    // Parses `git status --porcelain=v1` output. Each line is "XY PATH",
    // where a rename/copy line reads "XY ORIG_PATH -> NEW_PATH"; the
    // reported path is the new one.
    std::vector<VcsFileStatus> parseStatusPorcelain(const std::string& output)
    {
        std::vector<VcsFileStatus> entries;

        std::istringstream lines(output);
        std::string line;
        while (std::getline(lines, line)) {
            if (line.size() <= kPorcelainPathOffset) { continue; }

            VcsFileStatus entry;
            entry.indexState    = toFileState(line[0]);
            entry.workTreeState = toFileState(line[1]);

            auto path = line.substr(kPorcelainPathOffset);
            if (const auto arrowPosition = path.find(" -> ");
                arrowPosition != std::string::npos) {
                path = path.substr(arrowPosition + std::string(" -> ").size());
            }
            entry.path = std::move(path);

            entries.push_back(std::move(entry));
        }

        return entries;
    }

    VcsError classifyError(const VcsProcessResult& result)
    {
        if (result.crashed) { return VcsError::Other; }
        if (result.exitCode == 0) { return VcsError::None; }
        if (result.standardError.find("not a git repository") !=
            std::string::npos) {
            return VcsError::NotARepository;
        }
        return VcsError::Other;
    }

    std::vector<std::string> withPathArguments(
        std::vector<std::string> subcommand,
        const std::vector<std::string>& paths)
    {
        subcommand.insert(subcommand.end(), paths.begin(), paths.end());
        return subcommand;
    }
} // namespace

VcsRepository::VcsRepository(std::string rootPath,
                             VcsProcessRunnerInterfacePtr runner,
                             GitExecutableResolver gitExecutableResolver,
                             LoggerPtr logger)
    : m_rootPath(std::move(rootPath))
    , m_runner(std::move(runner))
    , m_gitExecutableResolver(std::move(gitExecutableResolver))
    , m_logger(std::move(logger))
{}

void VcsRepository::runGit(
    const std::vector<std::string>& arguments,
    std::function<void(VcsError, VcsProcessResult)> onComplete)
{
    const auto gitExecutable =
        m_gitExecutableResolver ? m_gitExecutableResolver() : std::nullopt;
    if (!gitExecutable.has_value()) {
        if (m_logger) {
            m_logger->warn(
                "Cannot run git command: no git executable "
                "resolved");
        }
        onComplete(VcsError::GitNotFound, VcsProcessResult{});
        return;
    }

    m_runner->run(
        m_rootPath, *gitExecutable, arguments,
        [onComplete = std::move(onComplete)](VcsProcessResult result) {
            const auto error = classifyError(result);
            onComplete(error, std::move(result));
        });
}

void VcsRepository::runOperation(const std::vector<std::string>& arguments,
                                 OperationCallback callback)
{
    runGit(arguments, [callback = std::move(callback)](
                          VcsError error, VcsProcessResult result) {
        callback(
            VcsOperationResult{.error   = error,
                               .details = std::move(result.standardError)});
    });
}

void VcsRepository::status(StatusCallback callback)
{
    runGit({"status", "--porcelain=v1"}, [callback = std::move(callback)](
                                             VcsError error,
                                             const VcsProcessResult& result) {
        VcsStatusResult statusResult;
        statusResult.error = error;
        if (error == VcsError::None) {
            statusResult.entries = parseStatusPorcelain(result.standardOutput);
        }
        callback(std::move(statusResult));
    });
}

void VcsRepository::stage(const std::vector<std::string>& paths,
                          OperationCallback callback)
{
    runOperation(withPathArguments({"add", "--"}, paths), std::move(callback));
}

void VcsRepository::unstage(const std::vector<std::string>& paths,
                            OperationCallback callback)
{
    runOperation(withPathArguments({"reset", "--"}, paths),
                 std::move(callback));
}

void VcsRepository::commit(const std::string& message,
                           OperationCallback callback)
{
    runOperation({"commit", "-m", message}, std::move(callback));
}

const std::string& VcsRepository::rootPath() const
{
    return m_rootPath;
}
