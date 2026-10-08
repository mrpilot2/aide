#ifndef AIDE_VCS_ERROR_HPP
#define AIDE_VCS_ERROR_HPP

#include <cstdint>

namespace aide
{
    /**
     * @brief Error outcomes for aide::vcs operations (#169).
     *
     * Populated so far (#171): None, GitNotFound, NotARepository, Other.
     * AuthenticationFailed, MergeConflict and NetworkError are reserved for
     * the pull/push work landing in a later ticket.
     */
    enum class VcsError : std::uint8_t
    {
        None,
        GitNotFound,
        NotARepository,
        AuthenticationFailed,
        MergeConflict,
        NetworkError,
        Other
    };
} // namespace aide

#endif // AIDE_VCS_ERROR_HPP
