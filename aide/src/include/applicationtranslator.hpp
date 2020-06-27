#ifndef AIDE_APPLICATIONTRANSLATOR_HPP
#define AIDE_APPLICATIONTRANSLATOR_HPP

#include <QTranslator>

#include <gui/translatorinterface.hpp>

namespace aide::gui
{
    class ApplicationTranslator : public TranslatorInterface
    {
    public:
        ApplicationTranslator();

        [[nodiscard]] std::vector<std::string> getAvailableTranslations()
            const override;

    private:
        QTranslator m_qtTranslator;
        QTranslator m_translator;
    };
} // namespace aide::gui

#endif // AIDE_APPLICATIONTRANSLATOR_HPP
