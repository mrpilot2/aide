#ifndef AIDE_APPLICATIONTRANSLATOR_HPP
#define AIDE_APPLICATIONTRANSLATOR_HPP

#include <gui/translatorinterface.hpp>

#include <QTranslator>

namespace aide::gui
{
    class ApplicationTranslator : public TranslatorInterface
    {
    public:
        ApplicationTranslator();

    private:
        QTranslator m_qtTranslator;
        QTranslator m_translator;
    };
} // namespace aide::gui

#endif // AIDE_APPLICATIONTRANSLATOR_HPP
