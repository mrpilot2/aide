
#ifndef AIDE_DESKTOP_ENTRY_NAME_RESOLVER_HPP
#define AIDE_DESKTOP_ENTRY_NAME_RESOLVER_HPP

#include <optional>
#include <string>

namespace aide::core
{
    // Pure parsing of a freedesktop .desktop file's [Desktop Entry] Name for
    // a given locale - no filesystem or subprocess access - so it stays
    // independently testable from the I/O that locates and reads the real
    // file (see OsFileManagerLauncher).
    class DesktopEntryNameResolver
    {
    public:
        // Resolves the display name from raw .desktop file content for
        // localeName (e.g. "de_DE"): an exact Name[de_DE] match wins, then
        // the language-only Name[de], then the untranslated Name=. Only the
        // [Desktop Entry] section is considered. Returns std::nullopt when
        // no Name entry of any kind is present. Encoding/modifier locale
        // variants (e.g. "de_DE.UTF-8", "ca@valencia") from the full Desktop
        // Entry Specification are intentionally not implemented.
        [[nodiscard]] static std::optional<std::string> resolve(
            const std::string& desktopEntryContent,
            const std::string& localeName);
    };
} // namespace aide::core

#endif // AIDE_DESKTOP_ENTRY_NAME_RESOLVER_HPP
