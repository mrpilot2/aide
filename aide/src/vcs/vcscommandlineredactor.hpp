#ifndef AIDE_VCS_VCS_COMMAND_LINE_REDACTOR_HPP
#define AIDE_VCS_VCS_COMMAND_LINE_REDACTOR_HPP

#include <string>

#include <string_view>

namespace aide
{
    // Redacts the password/token half of any embedded URL credential
    // (`user:pass@host` or `scheme://user:pass@host`) in commandLine,
    // keeping the username but replacing the secret with "***". Used before
    // logging any constructed git command line, since remote URLs may carry
    // credentials (e.g. `push https://user:token@host/repo.git`).
    [[nodiscard]] std::string redactVcsCommandLineCredentials(
        std::string_view commandLine);
} // namespace aide

#endif // AIDE_VCS_VCS_COMMAND_LINE_REDACTOR_HPP
