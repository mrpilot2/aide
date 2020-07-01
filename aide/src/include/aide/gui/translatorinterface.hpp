#ifndef AIDE_TRANSLATORINTERFACE_HPP
#define AIDE_TRANSLATORINTERFACE_HPP

#include <set>
#include <string>

namespace aide::gui
{
    class TranslatorInterface
    {
    public:
        virtual ~TranslatorInterface() = default;

        virtual std::set<std::string> getAvailableTranslations() const = 0;
    };
} // namespace aide::gui

#endif // AIDE_TRANSLATORINTERFACE_HPP
