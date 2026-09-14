#include "vcscommandlineredactor.hpp"

#include <regex>

std::string aide::redactVcsCommandLineCredentials(std::string_view commandLine)
{
    // Matches an optional "scheme://" followed by "user:secret@", e.g.
    // "https://user:token@host/repo.git" or "user:pass@host/repo.git".
    // Login/password characters exclude ':', '/', '@' and whitespace so the
    // match stays anchored to the actual credential segment.
    static const std::regex kCredentialPattern(
        R"(([A-Za-z][A-Za-z0-9+.-]*://)?([^\s:/@]+):([^\s:/@]+)@)");

    return std::regex_replace(std::string(commandLine), kCredentialPattern,
                              "$1$2:***@");
}
