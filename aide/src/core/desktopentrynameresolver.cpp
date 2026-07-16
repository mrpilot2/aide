
#include "desktopentrynameresolver.hpp"

#include <sstream>

#include <unordered_map>

using aide::core::DesktopEntryNameResolver;

namespace
{
    constexpr std::string_view kDesktopEntrySection = "[Desktop Entry]";
    constexpr std::string_view kNameKey             = "Name";
    constexpr std::string_view kLocalizedNamePrefix = "Name[";

    std::string trim(const std::string& value)
    {
        const auto begin = value.find_first_not_of(" \t\r\n");
        if (begin == std::string::npos) { return {}; }
        const auto end = value.find_last_not_of(" \t\r\n");
        return value.substr(begin, end - begin + 1);
    }

    std::string languagePart(const std::string& localeName)
    {
        const auto underscorePos = localeName.find('_');
        return underscorePos == std::string::npos
                   ? localeName
                   : localeName.substr(0, underscorePos);
    }
} // namespace

std::optional<std::string> DesktopEntryNameResolver::resolve(
    const std::string& desktopEntryContent, const std::string& localeName)
{
    std::optional<std::string> defaultName;
    std::unordered_map<std::string, std::string> localizedNames;

    bool inDesktopEntrySection = false;
    std::istringstream stream(desktopEntryContent);
    std::string rawLine;
    while (std::getline(stream, rawLine)) {
        const auto line = trim(rawLine);
        if (line.empty() || line.front() == '#') { continue; }
        if (line.front() == '[') {
            inDesktopEntrySection = (line == kDesktopEntrySection);
            continue;
        }
        if (!inDesktopEntrySection) { continue; }

        const auto equalsPos = line.find('=');
        if (equalsPos == std::string::npos) { continue; }
        const auto key   = trim(line.substr(0, equalsPos));
        const auto value = trim(line.substr(equalsPos + 1));

        if (key == kNameKey) {
            defaultName = value;
        } else if (key.starts_with(kLocalizedNamePrefix) && key.back() == ']') {
            const auto locale =
                key.substr(kLocalizedNamePrefix.size(),
                           key.size() - kLocalizedNamePrefix.size() - 1);
            localizedNames[locale] = value;
        }
    }

    if (const auto exactMatch = localizedNames.find(localeName);
        exactMatch != localizedNames.end()) {
        return exactMatch->second;
    }
    if (const auto languageMatch =
            localizedNames.find(languagePart(localeName));
        languageMatch != localizedNames.end()) {
        return languageMatch->second;
    }
    return defaultName;
}
